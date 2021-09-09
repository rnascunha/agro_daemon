#include <cstdio>
#include <iostream>
#include <functional>
#include <filesystem>

#include <boost/asio.hpp>

#include "arguments.hpp"

#include "tt/tt.hpp"

#include "coap_engine.hpp"
#include "websocket/types.hpp"

#include "device/list.hpp"
#include "notify/notify_request.hpp"
#include "resources/init.hpp"

#include "db/db.hpp"
#include "agro.hpp"

#if USE_SSL == 1
#include "websocket/load_certificate.hpp"
#endif /* USE_SSL == 1 */

static constexpr const std::string_view subscriber{"email@email.com"};

static pusha::key get_notify_key(std::filesystem::path const& path, Agro::DB& db) noexcept
{
	pusha::key key;

	if(!path.empty())
	{
		if(!std::filesystem::is_regular_file(path))
		{
			tt::error("notify private key: '%s' is not a valid file.", path.c_str());
			goto import_db;
		}
		else if(!key.import(path))
		{
			tt::error("notify private key: Failed to import from '%s' file.", path.c_str());
			goto import_db;
		}
		else {
			tt::debug("notify private key: imported from '%s' file.", path.c_str());
			goto end;
		}
	}
	else
	{
import_db:
		std::string keyb64 = db.notify_private_key();
		if(!keyb64.empty())
		{
			if(!key.import(keyb64))
			{
				tt::error("notifiy private key: import from 'DB' fail.");
			}
			else
			{
				tt::debug("notify private key: key imported from DB.");
			}
		}
	}

end:
	return key;
}

int main(int argc, char** argv)
{
	arguments args;
	read_parameters(argc, argv, args);

	std::error_code ecb;
	Agro::DB db{args.db_file.c_str(), ecb};
	if(ecb)
	{
		tt::error("Error opening DB %s", args.db_file.c_str());
		return 1;
	}

//	init_ota();
//	init_app();

	boost::system::error_code ec;
	auto const address = boost::asio::ip::make_address(args.addr, ec);
	if(ec)
	{
		tt::error("Invalid address! [%s]", args.addr.c_str());
		return EXIT_FAILURE;
	}
	auto port = static_cast<unsigned short>(args.port);

	// The io_context is required for all I/O
	boost::asio::io_context ioc;

	std::error_code sec;
	pusha::key key = get_notify_key(args.notify_priv_key, db);
	if(sec)
	{
		tt::error("Error opening notify private key! [%s]", sec.message().c_str());
	}
	notify_factory factory{ioc, std::move(key), subscriber};

	tt::status("Init code");

	udp_conn conn;

	CoAP::Error ecp;
	udp_conn::endpoint ep{args.coap_addr.c_str(), static_cast<unsigned short>(args.coap_port), ecp};
	if(ecp)
	{
		tt::error("Invalid device endpoint! [%s:%d]", args.coap_addr.c_str(), args.coap_port);
		return EXIT_FAILURE;
	}

	conn.open(ecp);
	if(ecp)
	{
		tt::error("Error opening device socket! [%d/%s]", ecp.value(), ecp.message());
		return EXIT_FAILURE;
	}

	conn.bind(ep, ecp);
	if(ecp)
	{
		tt::error("Error binding device socket! [%d/%s]", ecp.value(), ecp.message());
		return EXIT_FAILURE;
	}

	Device_List device_list;
	engine coap_engine{std::move(conn), CoAP::Message::message_id{CoAP::random_generator()}};

	Agro::instance app{db, coap_engine, device_list, factory};
	auto sh = std::make_shared<Agro::share>(app);

	std::vector<engine::resource_node> vresource;
	Resource::init(coap_engine, device_list, sh, vresource);

#if USE_SSL == 0
	Agro::make_listener<Agro::websocket>(ioc, address, port, sh, ec);
#else /* USE_SSL == 0 */
	boost::asio::ssl::context ctx{boost::asio::ssl::context::tlsv12};
	Agro::load_certificate(ctx, args.cert, args.key, ec);
	if(ec)
	{
		tt::error("Error loading key/certificate! [%d/%s]", ec.value(), ec.message());
		return EXIT_FAILURE;
	}

	Agro::make_listener<Agro::websocket>(ioc, ctx, address, port, sh, ec);
#endif /* USE_SSL == 0 */
	if(ec)
	{
		tt::error("Error opening listener! [%d/%s]", ec.value(), ec.message());
		return EXIT_FAILURE;
	}

	// Capture SIGINT and SIGTERM to perform a clean shutdown
	boost::asio::signal_set signals(ioc, SIGINT, SIGTERM);
	bool keep_running = true;
	signals.async_wait(
		[&ioc, &coap_engine, &keep_running](boost::system::error_code const&, int){
			ioc.stop();
			coap_engine.get_connection().close();
			keep_running = false;
		});


	// Run the I/O service on the requested number of threads
	std::vector<std::thread> v;
	v.reserve(args.num_threads - 1);
	for(auto i = args.num_threads - 1; i > 0; --i)
		v.emplace_back(
		[&ioc]
		{
			ioc.run();
		});

	while(keep_running)
	{
		coap_engine.run<50>(ecp);
		ioc.run_for(std::chrono::milliseconds(50));
	}

	// Block until all the threads exit
	for(auto& t : v)
		t.join();

	if(ecp && ecp != CoAP::errc::socket_receive)
	{
		tt::error("Error running! [%d/%s]", ecp.value(), ecp.message());
	}

	return EXIT_SUCCESS;
}
