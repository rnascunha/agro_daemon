#include "boost/asio.hpp"
#include "coap_engine.hpp"
#include <cstdio>
#include <iostream>
#include "tt/tt.hpp"
#include "websocket.hpp"

#define WEBSOCKET_ADDRESS		"0.0.0.0"
#define WEBSOCKET_PORT			8081

template<typename ErrorType>
void print_error(ErrorType ec, const char* what = "")
{
	std::cerr << "ERROR! [" << ec.value() << "] "
			<< ec.message() << " [" << what << "]\n";
}

int main()
{
	boost::system::error_code ec;
	auto const address = boost::asio::ip::make_address(WEBSOCKET_ADDRESS, ec);
	if(ec)
	{
		print_error(ec, "address");
		return EXIT_FAILURE;
	}
	auto port = static_cast<unsigned short>(WEBSOCKET_PORT);

	// The io_context is required for all I/O
	boost::asio::io_context ioc;

	My_Async::Listener::make_listener<Websocket<false>>(ioc, address, port, ec);
	if(ec)
	{
		print_error(ec, "open listener");
		return EXIT_FAILURE;
	}

	std::cout << "Socket opened\n";

	    // Capture SIGINT and SIGTERM to perform a clean shutdown
	boost::asio::signal_set signals(ioc, SIGINT, SIGTERM);
	signals.async_wait(
		[&ioc](boost::system::error_code const&, int){
			ioc.stop();
		});

	tt::status("Init code");

	udp_conn conn;

	udp_conn::endpoint ep{5655};
	CoAP::Error ecp;
	conn.open(ecp);
	if(ec)
	{
		print_error(ecp, "open");
		return EXIT_FAILURE;
	}

	conn.bind(ep, ecp);
	if(ec)
	{
		print_error(ecp, "bind");
		return EXIT_FAILURE;
	}

	engine coap_engine{std::move(conn), CoAP::Message::message_id{CoAP::random_generator()}};

	while(coap_engine.run<50>(ecp))
	{
		ioc.run_for(std::chrono::milliseconds(50));
	}

	if(ec)
	{
		print_error(ecp, "run");
	}

	return EXIT_SUCCESS;
}
