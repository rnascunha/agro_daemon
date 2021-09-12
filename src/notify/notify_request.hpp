#ifndef AGRO_DAEMON_NOTIFY_REQUEST_HPP__
#define AGRO_DAEMON_NOTIFY_REQUEST_HPP__

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>

#include <iostream>
#include <memory>

#include "pusha.h"
#include "pusha.hpp"

#include "../user/user.hpp"

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
		notify_factory(
				boost::asio::io_context& ioc,
				pusha::key&& ec_key,
				std::string_view const& subscriber);

		bool notify(Agro::User::Subscription const&,
				std::uint8_t const* payload, std::size_t payload_len,
				unsigned expiration = 0, unsigned ttl = 0) noexcept;
		bool notify(Agro::User::Subscription const&,
					std::string const& data,
					unsigned expiration = 0, unsigned ttl = 0) noexcept;

		bool is_valid() const noexcept;
		std::string_view const& public_key() const noexcept;
	private:
		boost::asio::io_context& ioc_;
		boost::asio::ssl::context ctx_{boost::asio::ssl::context::sslv23};
		bool is_valid_;
		std::string_view public_key_;
		pusha::notify push_;
};

std::string make_public_key(std::string_view const& public_key) noexcept;

#endif /* AGRO_DAEMON_NOTIFY_REQUEST_HPP__ */
