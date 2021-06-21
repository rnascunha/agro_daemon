//#include "boost/asio.hpp"
#include "coap_engine.hpp"
#include <cstdio>
#include <iostream>
#include "tt/tt.hpp"

#define WEBSOCKET_ADDRESS		"0.0.0.0"
#define WEBSOCKET_PORT			8080

void print_error(CoAP::Error ec, const char* what = "")
{
	std::printf("ERRRO! [%d] %s [%s]", ec.value(), ec.message(), what);
}

int main()
{
	/*
	boost::system::error_code ec;
	auto const address = boost::asio::ip::make_address(WEBSOCKET_ADDRESS, ec);
	if(ec)
	{
		std::cerr << "ERROR! [" << WEBSOCKET_ADDRESS << "] Invalid address\n";
		return EXIT_FAILURE;
	}
	auto port = static_cast<unsigned short>(WEBSOCKET_PORT);

	// The io_context is required for all I/O
	boost::asio::io_context ioc;

	My_Async::Listener::make_listener<Core::Propagator>(ioc, address, port, ec);
	if(ec)
	{
		std::cerr << "Error Listener: " << ec.value() << "|" << ec.message() << "\n";
		return EXIT_FAILURE;
	}

	std::cout << "Socket opened\n";

	    // Capture SIGINT and SIGTERM to perform a clean shutdown
	boost::asio::signal_set signals(ioc, SIGINT, SIGTERM);
	signals.async_wait(
		[&ioc](boost::system::error_code const&, int){
			ioc.stop();
		});

	ioc.run();
	*/
	tt::status("Init code");

	udp_conn conn;

	udp_conn::endpoint ep{5655};
	CoAP::Error ec;
	conn.open(ec);
	if(ec)
	{
		print_error(ec, "open");
		exit(1);
	}

	conn.bind(ep, ec);
	if(ec)
	{
		print_error(ec, "bind");
		exit(1);
	}

	engine coap_engine{std::move(conn), CoAP::Message::message_id{CoAP::random_generator()}};

	while(coap_engine.run<50>(ec))
	{

	}

	if(ec)
	{
		print_error(ec, "run");
	}

	return EXIT_SUCCESS;
}
