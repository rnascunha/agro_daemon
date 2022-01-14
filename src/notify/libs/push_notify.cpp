#include "push_notify.hpp"
#include <iostream>

#define PUSH_DEFAULT_EXAPIRATION	(12 * 60 * 60)
#define PUSH_DEFAULT_TTL			60

push_notify::push_notify(boost::asio::io_context& io_context,
					boost::asio::ssl::context& context,
					const std::string& endpoint,
					std::uint8_t* request, std::size_t request_len)
: socket_(io_context, context),
  endpoint_(endpoint),
  resolver_(io_context),
  payload_(request), payload_len_(request_len)
{}

push_notify::~push_notify()
{
	free(payload_);
}

//void notify_request::connect(const tcp::resolver::results_type& endpoints)
void push_notify::connect()
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

void push_notify::do_connect(const boost::asio::ip::tcp::resolver::results_type& endpoints)
{
	auto self = shared_from_this();
	boost::asio::async_connect(
			socket_.lowest_layer(),
			endpoints,
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

void push_notify::handshake()
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

void push_notify::send_request()
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

void push_notify::receive_response()
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
					std::cout << "[push] Reply: " << std::string{str, (size_t)(std::strchr(str, '\r') - str)} << "\n";
				}
			}
			else
			{
				std::cout << "[push] failed: " << error.message() << "\n";
			}
		}
	);
}

push_factory::push_factory(boost::asio::io_context& ioc,
		pusha::key&& ec_key,
		std::string_view const& subscriber)
	: ioc_{ioc},
	  is_valid_{ec_key.get_key() != NULL},
	  public_key_{is_valid_ ? ec_key.export_public_key() : ""},
	  push_{std::move(ec_key), subscriber}
{}

bool push_factory::notify(Agro::User::Subscription const& user,
			std::uint8_t const* payload, std::size_t payload_len,
			unsigned expiration /* = 0 */, unsigned ttl /* = 0 */) noexcept
{
	if(!is_valid_) return false;

	if(!expiration)
	{
		expiration = std::time(NULL) + PUSH_DEFAULT_EXAPIRATION;
	}

	if(!ttl)
	{
		ttl = PUSH_DEFAULT_TTL;
	}

	pusha_http_headers headers = {};
	pusha_payload pp = {};
	int err = push_.make(headers,
			&pp,
			user.endpoint(),
			user.p256dh(),
			user.auth(),
			expiration,
			payload, payload_len);

	if(err)
	{
		std::cerr << "Error making 'pusha' http request\n";
		return false;
	}

	std::size_t packet_size;
	std::uint8_t * packet = http_request_serialize(user.endpoint().data(),
			user.endpoint().size(),
			&headers,
			pp.cipher_payload, pp.cipher_payload_len,
			&packet_size);
	free_pusha_http_headers(&headers);
	free_pusha_payload(&pp);

	if(!packet)
	{
		std::cerr << "Error serializing notify packet\n";
		return false;
	}
	std::cout << "Notify serialized [" << packet_size << "]\n";

	std::make_shared<push_notify>(ioc_, ctx_, user.endpoint(), packet, packet_size)->connect();

	return true;
}

bool push_factory::notify(Agro::User::Subscription const& user,
					std::string const& data,
					unsigned expiration /* = 0 */, unsigned ttl /* = 0 */) noexcept
{
	return notify(user, reinterpret_cast<std::uint8_t const*>(data.data()), data.size(), expiration, ttl);
}

bool push_factory::is_valid() const noexcept
{
	return is_valid_;
}

std::string_view const& push_factory::public_key() const noexcept
{
	return public_key_;
}
