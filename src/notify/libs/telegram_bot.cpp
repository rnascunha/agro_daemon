#if _MSC_VER
#include <SDKDDKVer.h>
#endif /* _MSC_VER */

#include "telegram_bot.hpp"
#include "pusha/helper.h"

#include <iostream>

telegram_bot::telegram_bot(boost::asio::io_context& io_context,
		boost::asio::ssl::context& context,
		std::string const& token,
		std::string const& chatid,
		std::string const& message)
: socket_(io_context, context),
  resolver_(io_context)
{
	std::ostringstream ss;

	ss << "GET "
		<< "/bot" << token
		<< "/sendMessage?chat_id="
		<< chatid << "&parse_mode=Markdown&text="
		<< message << " HTTP/1.1\r\n"
		<< "Host: " << host << "\r\n"
		<< "Accept: */*\r\n\r\n";

	payload_ = ss.str();
}

telegram_bot::~telegram_bot(){}

void telegram_bot::connect()
{
	auto self = shared_from_this();
	resolver_.async_resolve(std::string_view{host}, std::string_view{"443"},
			[self](const boost::system::error_code& error, // Result of operation.
		boost::asio::ip::tcp::resolver::results_type results)
		{
			if(!error)
			{
				self->do_connect(results);
			}
			else
			{
				std::cout << "Resolve failed: " << error.message() << "\n";
			}
		});
}

void telegram_bot::do_connect(const boost::asio::ip::tcp::resolver::results_type& endpoints)
{
	auto self = shared_from_this();
	boost::asio::async_connect(socket_.lowest_layer(), endpoints,
		[self](const boost::system::error_code& error,
				const tcp::endpoint& /*endpoint*/)
		{
			if (!error)
			{
				self->handshake();
			}
			else
			{
				std::cout << "Connect failed: " << error.message() << "\n";
			}
		}
	);
}

void telegram_bot::handshake()
{
	auto self = shared_from_this();
	socket_.async_handshake(boost::asio::ssl::stream_base::client,
		[self](const boost::system::error_code& error)
		{
			if (!error)
			{
				self->send_request();
			}
			else
			{
				std::cout << "Handshake failed: " << error.message() << "\n";
			}
		}
	);
}

void telegram_bot::send_request()
{
	auto self = shared_from_this();
	boost::asio::async_write(socket_,
			boost::asio::buffer(payload_),
		[self](const boost::system::error_code& error, std::size_t)
		{
			if (!error)
			{
				self->receive_response();
			}
			else
			{
				std::cerr << "Write failed: " << error.message() << "\n";
			}
		}
	);
}

void telegram_bot::receive_response()
{
	auto self = shared_from_this();
	socket_.async_read_some(boost::asio::buffer(reply_, max_length),
		[self](const boost::system::error_code& error, std::size_t)
		{
			if (!error)
			{
				char* str = strstr(self->reply_, "HTTP/1.1 ");
				if(str)
				{
					str += std::strlen("HTTP/1.1 ");
					std::cout << "[telegram] Reply: " << std::string{str, (size_t)(std::strchr(str, '\r') - str)} << "\n";
				}
			}
			else
			{
				std::cout << "[telegram] Failed: " << error.message() << "\n";
			}
		}
	);
}

telegram_factory::telegram_factory(boost::asio::io_context& ioc,
		std::string const& token)
	: ioc_{ioc}, token_{token}{}

bool telegram_factory::is_valid() const noexcept
{
	return !token_.empty();
}

void telegram_factory::notify(std::string const& chatid, std::string const& message) noexcept
{
	if(chatid.empty()) return;
	std::make_shared<telegram_bot>(ioc_, ctx_, token_, chatid, message)->connect();
}

std::string const& telegram_factory::token() const noexcept
{
	return token_;
}

void telegram_factory::token(std::string const& ntoken) noexcept
{
	token_ = ntoken;
}
