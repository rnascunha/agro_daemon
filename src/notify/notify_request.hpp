#ifndef AGRO_DAEMON_NOTIFY_REQUEST_HPP__
#define AGRO_DAEMON_NOTIFY_REQUEST_HPP__

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <iostream>
#include <memory>
#include "pusha.h"
#include "pusha.hpp"

#include <cstdint>

using boost::asio::ip::tcp;

enum { max_length = 1024 };

class notify_request : public std::enable_shared_from_this<notify_request>
{
	public:
		notify_request(boost::asio::io_context& io_context,
					boost::asio::ssl::context& context,
					const std::string& endpoint,
					std::uint8_t* request, std::size_t request_len);

		~notify_request();

		void connect();
	private:
		void do_connect(const boost::asio::ip::tcp::resolver::results_type& results);

		void handshake();
		void send_request();
		void receive_response();

		boost::asio::ssl::stream<tcp::socket> socket_;
		std::string endpoint_;
		boost::asio::ip::tcp::resolver resolver_;
		std::uint8_t* payload_;
		std::size_t	payload_len_;
		char reply_[max_length];
};

class notify_factory{
	public:
		notify_factory(boost::asio::io_context& ioc);
		void request(std::string const& endpoint,
				std::uint8_t* payload, std::size_t payload_len);
	private:
		boost::asio::io_context& ioc_;
		boost::asio::ssl::context ctx_{boost::asio::ssl::context::sslv23};
};

#endif /* AGRO_DAEMON_NOTIFY_REQUEST_HPP__ */
