#include <cstdio>
#include <iostream>
#include "arguments.hpp"
#include <functional>

#include "boost/asio.hpp"

#include "tt/tt.hpp"

#include "coap_engine.hpp"
#include "websocket/websocket.hpp"

#include "device/list.hpp"
//#include "resources/init.hpp"

//#include "ota/ota.hpp"
//#include "app/app.hpp"
//
//#include "notify/notify.hpp"
//#include "notify/notify_request.hpp"

//#include "db/sqlite3.hpp"
//#include "user/user.hpp"
#include "db/db.hpp"
#include "agro.hpp"

template<typename ErrorType>
void print_error(ErrorType ec, const char* what = "")
{
	std::cerr << "ERROR! [" << ec.value() << "] "
			<< ec.message() << " [" << what << "]\n";
}

int main(int argc, char** argv)
{
	arguments args;
	read_parameters(argc, argv, args);

	std::error_code ecb;
	Agro::DB db{args.db_file.c_str(), ecb};
	if(ecb)
	{
		tt::error("Error opening DB %s\n", args.db_file.c_str());
		return 1;
	}

//	init_ota();
//	init_app();

	boost::system::error_code ec;
	auto const address = boost::asio::ip::make_address(args.addr, ec);
	if(ec)
	{
		print_error(ec, "address");
		return EXIT_FAILURE;
	}
	auto port = static_cast<unsigned short>(args.port);

	// The io_context is required for all I/O
	boost::asio::io_context ioc;

	My_Async::Listener::make_listener<Websocket<false>>(ioc, address, port, ec);
	if(ec)
	{
		print_error(ec, "open listener");
		return EXIT_FAILURE;
	}

	tt::status("Socket opened\n");

	notify_factory factory{ioc};
	init_notify(factory);

	tt::status("Init code");

	udp_conn conn;

	CoAP::Error ecp;
	udp_conn::endpoint ep{args.coap_addr.c_str(), static_cast<unsigned short>(args.coap_port), ecp};
	if(ecp)
	{
		print_error(ecp, "endpoint");
		return EXIT_FAILURE;
	}

	conn.open(ecp);
	if(ecp)
	{
		print_error(ecp, "open");
		return EXIT_FAILURE;
	}

	conn.bind(ep, ecp);
	if(ecp)
	{
		print_error(ecp, "bind");
		return EXIT_FAILURE;
	}

	Device_List device_list;

	engine coap_engine{std::move(conn), CoAP::Message::message_id{CoAP::random_generator()}};

//	std::vector<engine::resource_node> vresource;
//	Resource::init(coap_engine, device_list, vresource);

	Agro::instance app{db, coap_engine, device_list};
	Websocket<false>::data(app);

	// Capture SIGINT and SIGTERM to perform a clean shutdown
	boost::asio::signal_set signals(ioc, SIGINT, SIGTERM);
	signals.async_wait(
		[&ioc, &coap_engine](boost::system::error_code const&, int){
			ioc.stop();
			coap_engine.get_connection().close();
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
	ioc.run();

	while(coap_engine.run<50>(ecp))
	{
		ioc.run_for(std::chrono::milliseconds(50));
	}

	// Block until all the threads exit
	for(auto& t : v)
		t.join();

	if(ecp)
	{
		print_error(ecp, "run");
	}

	return EXIT_SUCCESS;
}
