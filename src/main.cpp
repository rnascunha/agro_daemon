#include <boost/asio.hpp>

#include "arguments.hpp"
#include "tt/tt.hpp"
#include "user/user.hpp"
#include "instance/agro.hpp"

#if USE_SSL == 1
#include "websocket/load_certificate.hpp"
#endif /* USE_SSL == 1 */

static constexpr const std::string_view subscriber{"mailto:email@email.com"};

int main(int argc, char** argv)
{
	/**
	 * Reading paramters
	 */
	arguments args;
	read_parameters(argc, argv, args);

	/**
	 * Listener endpoint
	 */
	boost::system::error_code ec;
	auto const address = boost::asio::ip::make_address(args.addr, ec);
	if(ec)
	{
		tt::error("Invalid address! [%s]", args.addr.c_str());
		return EXIT_FAILURE;
	}
	auto port = static_cast<unsigned short>(args.port);
	boost::asio::ip::tcp::endpoint epl{address, port};

	/**
	 * Device endpoint
	 */
	CoAP::Error ecp;
	udp_conn::endpoint ep{args.coap_addr.c_str(), static_cast<unsigned short>(args.coap_port), ecp};
	if(ecp)
	{
		tt::error("Invalid device endpoint! [%s:%d]", args.coap_addr.c_str(), args.coap_port);
		return EXIT_FAILURE;
	}

	/**
	 * Initiating agro instance
	 */
	// The io_context is required for all I/O
	boost::asio::io_context ioc;

	std::error_code ecb;
	Agro::instance instance{ioc,
		args.db_file.c_str(),
		args.notify_priv_key, subscriber,
		args.telegram_bot_token,
		ep, epl,
#if USE_SSL == 1
		args.key, args.cert,
#endif /* USE_SSL == 1 */
		ecb};
	if(ecb)
	{
		return EXIT_FAILURE;
	}

	/**
	 * Running
	 */
	CoAP::Error ecc;
	instance.run<50>(args.num_threads, ecc);

	if(ecc && ecc != CoAP::errc::socket_receive)
	{
		tt::error("Error running! [%d/%s]", ecp.value(), ecp.message());
	}

	return EXIT_SUCCESS;
}
