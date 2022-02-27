#ifndef SMTP_CLIENT_IMPL_HPP__
#define SMTP_CLIENT_IMPL_HPP__

#include "../client.hpp"

#include <iostream>

#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/transform_width.hpp>
#include <boost/archive/iterators/binary_from_base64.hpp>

#ifndef DEBUG
#define DEBUG	0
#endif /* DEBUG */

#if DEBUG != 0
#define SCOPE_LOG()     	printf("%s\n", __PRETTY_FUNCTION__)
#define DEBUG_PRINT(...)	printf(__VA_ARGS__)
#else
#define SCOPE_LOG()
#define DEBUG_PRINT(...)
#endif

namespace SMTP{

[[maybe_unused]] static std::string base64_encode(const std::string& data)
{
    using namespace boost::archive::iterators;

    typedef base64_from_binary< transform_width< const char *, 6, 8 > > base64_text;

    std::string result;
    result.reserve( data.size() * 4 / 3  );
    std::copy(base64_text(data.c_str()), base64_text(data.c_str() + data.size()), std::back_inserter(result));
    std::string::size_type data_size = data.size();

    while( data_size % 3 != 0)
        (result += '='), (++data_size);

    return result;
}


template<bool UsePipeline,
		int TimeOut>
Client<UsePipeline, TimeOut>::Client(
		boost::asio::io_context& ioc,
		boost::asio::ssl::context& ctx)
	: ioc_{ioc},
	  socket_{ioc, ctx},
	  resolver_{ioc},
	  timer_{ioc}
{
    SCOPE_LOG();
}

template<bool UsePipeline,
		int TimeOut>
Client<UsePipeline, TimeOut>::~Client()
{
    SCOPE_LOG();
}

template<bool UsePipeline,
		int TimeOut>
void Client<UsePipeline, TimeOut>::set_mail_server(const server& server) noexcept
{
    SCOPE_LOG();
    server_ = server;
}

template<bool UsePipeline,
		int TimeOut>
void Client<UsePipeline, TimeOut>::set_mail_data(const data& data) noexcept
{
	SCOPE_LOG();
	data_ = data;
}

template<bool UsePipeline,
		int TimeOut>
void Client<UsePipeline, TimeOut>::connect() noexcept
{
	using namespace std::placeholders;
	boost::asio::ip::tcp::resolver::query qry{
			server_.server,
			server_.port,
			boost::asio::ip::resolver_query_base::numeric_service};
	resolver_.async_resolve(qry,
			std::bind(
					&Client::on_resolve,
					this->shared_from_this(),
					_1,
					_2));
}

template<bool UsePipeline,
		int TimeOut>
void Client<UsePipeline, TimeOut>::on_resolve(
		const boost::system::error_code& ec,
		const boost::asio::ip::tcp::resolver::results_type& endpoints) noexcept
{
    SCOPE_LOG();
    if(ec)
    {
    	error(ec, "on_resolve");
		return;
    }

    using namespace std::placeholders;

    boost::asio::async_connect(
    		socket_.lowest_layer(),
    		endpoints,
			std::bind(
					&Client::on_connect,
					this->shared_from_this(),
					_1,
					_2));
}

template<bool UsePipeline,
		int TimeOut>
void Client<UsePipeline, TimeOut>::on_connect(const boost::system::error_code& ec,
		const boost::asio::ip::tcp::endpoint& /*endpoint*/) noexcept
{
    SCOPE_LOG();
    if (ec)
    {
    	error(ec, "on_connect");
        return ;
    }

    socket_.async_handshake(boost::asio::ssl::stream_base::client,
    		std::bind(&Client::on_handshake,
    				this->shared_from_this(),
					std::placeholders::_1) ) ;
}

template<bool UsePipeline,
		int TimeOut>
void Client<UsePipeline, TimeOut>::on_handshake(const boost::system::error_code& ec) noexcept
{
    SCOPE_LOG();
    if (ec)
    {
    	error(ec, "on_handshake");
        return ;
    }

    build_request();

    timer_.expires_from_now( boost::posix_time::seconds(TimeOut));
	timer_.async_wait(std::bind(&Client::on_timeout,
			this->shared_from_this(),
			std::placeholders::_1));

    on_send_command(0, ec);
}

template<bool UsePipeline,
		int TimeOut>
void Client<UsePipeline, TimeOut>::on_receive_command(std::size_t index, const boost::system::error_code& ec) noexcept
{
	SCOPE_LOG();
	if (ec)
	{
		error(ec, "on_receive_command");
		return ;
	}

	std::stringstream stream ;
	stream << &response_;

	int response_code;
	stream >> response_code;

	DEBUG_PRINT("Response received: %s\n", stream.str().c_str());
	if(response_code != comm_[index].response_ok)
	{
		DEBUG_PRINT("Rensponse code error: recv/expect = %d/%d\n", response_code, comm_[index].response_ok);
		boost::system::error_code ecc = boost::system::errc::make_error_code(boost::system::errc::bad_message);
		error(ecc, "response_error");
		return;
	}

	if(index < (comm_.size() - 1))
	{
		if(!comm_[index].comm.empty())
		{
			DEBUG_PRINT("Sending: %s\n", comm_[index].comm.c_str());

			std::ostream os(&request_);
			os << comm_[index].comm;

			boost::asio::async_write(socket_,
				request_,
				std::bind(
						&Client::on_send_command,
						this->shared_from_this(),
						++index,
						std::placeholders::_1));
		}
		else
		{
			boost::system::error_code ec;
			on_send_command(++index, ec);
		}
	}
	else
	{
		timer_.cancel();
	}
}

template<bool UsePipeline,
		int TimeOut>
void Client<UsePipeline, TimeOut>::on_send_command(std::size_t index, const boost::system::error_code& ec) noexcept
{
	SCOPE_LOG();
	if (ec)
	{
		error(ec, "on_send_command");
		return ;
	}

	boost::asio::async_read(
			socket_,
			response_,
			boost::asio::transfer_at_least(1),
			std::bind(
					&Client::on_receive_command, this->shared_from_this(),
					index,
					std::placeholders::_1));
}

template<bool UsePipeline,
		int TimeOut>
void Client<UsePipeline, TimeOut>::on_timeout(boost::system::error_code ec) noexcept
{
    SCOPE_LOG();
    if(!ec)
    {
    	DEBUG_PRINT("timeout\n");
        boost::system::error_code ignore_ec;
        socket_.lowest_layer().cancel(ignore_ec);
    }
    else
    {
    	/**
    	 * Cancelled.
    	 *
    	 * maybe because of error
    	 * maybe because it finished successfully
    	 */
    }
}

template<bool UsePipeline,
		int TimeOut>
void Client<UsePipeline, TimeOut>::error(const boost::system::error_code& ec,
		const char* what /* = "" */) noexcept
{
	std::cerr << "[mail] " << what << ": " << ec.message() << " [" << ec.value() << "]\n";

	boost::system::error_code ignore_ec;
	timer_.cancel(ignore_ec);
}

template<bool UsePipeline,
		int TimeOut>
void Client<UsePipeline, TimeOut>::build_request() noexcept
{
    SCOPE_LOG();
    std::ostringstream os;

    if constexpr(UsePipeline)
	{
		os << "ehlo " << server_.server << "\r\n";
		os << "auth login\r\n";
		os << base64_encode(server_.user) << "\r\n";
		os << base64_encode(server_.password) << "\r\n";
		os << "MAIL FROM: <" << data_.from_email << ">\r\n";
		for(auto const& to : data_.to_email)
		{
			os << "RCPT TO: <" << to <<  ">\r\n";
		}
		os << "DATA\r\n";
		os << data_.message << "\r\n.\r\n";

		comm_.emplace_back(os.str(), 220);
		comm_.emplace_back("", 250);
		comm_.emplace_back("", 334);
		comm_.emplace_back("", 334);
		comm_.emplace_back("", 235);

		auto s = data_.to_email.size();
//		for(auto const& to : data_.to_email)
		while(s--)
		{
			comm_.emplace_back("", 250);
		}
		comm_.emplace_back("", 250);
		comm_.emplace_back("", 354);
		comm_.emplace_back("", 250);
	}
    else
    {
		os << "ehlo " << server_.server << "\r\n";
		comm_.emplace_back(os.str(), 220);
		os.clear();
		os.str("");

		os << "auth login\r\n";
		comm_.emplace_back(os.str(), 250);
		os.clear();
		os.str("");

		os << base64_encode(server_.user) << "\r\n";
		comm_.emplace_back(os.str(), 334);
		os.clear();
		os.str("");

		os << base64_encode(server_.password) << "\r\n";
		comm_.emplace_back(os.str(), 334);
		os.clear();
		os.str("");

		os << "MAIL FROM: <" << data_.from_email << ">\r\n";
		comm_.emplace_back(os.str(), 235);
		os.clear();
		os.str("");

		for(auto const& to: data_.to_email)
		{
			os << "RCPT TO: <" << to <<  ">\r\n";
			comm_.emplace_back(os.str(), 250);
			os.clear();
			os.str("");
		}

		os << "DATA\r\n";
		comm_.emplace_back(os.str(), 250);
		os.clear();
		os.str("");

		os << data_.message << "\r\n.\r\n";
		comm_.emplace_back(os.str(), 354);
		os.clear();
		os.str("");

		comm_.emplace_back("", 250);
    }
}

}//SMTP

#endif /* SMTP_CLIENT_IMPL_HPP__ */
