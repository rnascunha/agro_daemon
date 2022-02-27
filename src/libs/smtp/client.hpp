#ifndef SMTP_CLIENT_HPP__
#define SMTP_CLIENT_HPP__

#include "boost/system/error_code.hpp"

#include "boost/asio.hpp"
#include "boost/asio/ssl.hpp"

#include <memory>
#include "types.hpp"

namespace SMTP{

struct data
{
    std::string					from_email;
    std::vector<std::string>	to_email;
    std::string 				message;
};

struct account
{
	account(std::string const& email_, std::string const& name_ = "")
		: email(email_), name(name_){}
	std::string	email;
	std::string	name;
};

template<bool UsePipeline = false,
		int TimeOut = 10 /* seconds */>
class Client : public std::enable_shared_from_this<Client<UsePipeline, TimeOut>>
{
	public:
		static constexpr bool use_pipeline = UsePipeline;
		static constexpr int timeout = TimeOut;

		Client(boost::asio::io_context&, boost::asio::ssl::context&);
		~Client();

		void set_mail_server(const server&) noexcept;
		void set_mail_data(const data&) noexcept;

		void connect() noexcept;
	private:
		struct command{
			command(std::string const& comm_, int resp_)
				: comm(comm_), response_ok(resp_){}
			std::string		comm;
			int				response_ok;
		};

		void on_resolve(const boost::system::error_code& ec,
				const boost::asio::ip::tcp::resolver::results_type& endpoints) noexcept;
		void on_connect(const boost::system::error_code& ec,
				const boost::asio::ip::tcp::endpoint&) noexcept;
		void on_handshake(const boost::system::error_code& ec) noexcept;

		void build_request() noexcept;

		void on_timeout(boost::system::error_code ec ) noexcept;

		void on_send_command(std::size_t index, const boost::system::error_code& ec) noexcept;
		void on_receive_command(std::size_t index, const boost::system::error_code& ec) noexcept;

		void error(const boost::system::error_code& ec,
				const char* what = "") noexcept;

		boost::asio::io_context& ioc_;
		boost::asio::ssl::stream<::boost::asio::ip::tcp::socket> socket_;

		boost::asio::ip::tcp::resolver resolver_;

		server   server_;
		data     data_;

		boost::asio::deadline_timer timer_;

		boost::asio::streambuf request_;
		boost::asio::streambuf response_;

		std::vector<command>	comm_;
};

std::string make_mail_message(
		account const& from,
		std::vector<account> const& tos,
		std::string const& subject,
		std::string const& body) noexcept;

void make_mail_data(
		data& data,
		account const& from,
		std::vector<account> const& tos,
		std::string const& subject,
		std::string const& body) noexcept;

}//SMTP

#include "impl/client_impl.hpp"

#endif /* SMTP_CLIENT_HPP__ */
