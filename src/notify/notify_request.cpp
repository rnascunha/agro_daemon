#include "notify_request.hpp"

notify_request::notify_request(boost::asio::io_context& io_context,
					boost::asio::ssl::context& context,
					const std::string& endpoint,
					std::uint8_t* request, std::size_t request_len)
: socket_(io_context, context),
  endpoint_(endpoint),
  resolver_(io_context),
  payload_(request), payload_len_(request_len)
{}

notify_request::~notify_request()
{
	free(payload_);
}

//void notify_request::connect(const tcp::resolver::results_type& endpoints)
void notify_request::connect()
{
	auto self = shared_from_this();
	char* host;
	std::size_t sep = host_path_separator(endpoint_.data(), endpoint_.size(), &host);
	resolver_.async_resolve(std::string_view{host, sep}, std::string_view{"443"},
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

void notify_request::do_connect(const boost::asio::ip::tcp::resolver::results_type& endpoints)
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

void notify_request::handshake()
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

void notify_request::send_request()
{
	auto self = shared_from_this();
	boost::asio::async_write(socket_,
			boost::asio::buffer(payload_, payload_len_),
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

void notify_request::receive_response()
{
	auto self = shared_from_this();
	socket_.async_read_some(boost::asio::buffer(reply_, max_length),
		[self](const boost::system::error_code& error, std::size_t length)
		{
			if (!error)
			{
				char* str = strstr(self->reply_, "HTTP/1.1 ");
				if(str)
				{
					str += std::strlen("HTTP/1.1 ");
					std::cout << "Reply: " << std::string{str, (size_t)(std::strchr(str, '\r') - str)} << "\n";
				}
			}
			else
			{
				std::cout << "Read failed: " << error.message() << "\n";
			}
		}
	);
}

notify_factory::notify_factory(boost::asio::io_context& ioc)
	: ioc_(ioc){}

void notify_factory::request(std::string const& endpoint,
				std::uint8_t* payload, std::size_t payload_len)
{
	std::make_shared<notify_request>(ioc_, ctx_, endpoint, payload, payload_len)->connect();
}
