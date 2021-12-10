#ifndef AGRO_DAEMON_NOTIFY_MAIL_HPP__
#define AGRO_DAEMON_NOTIFY_MAIL_HPP__

#include "smtp/client.hpp"

class mail_factory{
	public:
		mail_factory(boost::asio::io_context&,
				SMTP::server const&,
				std::string const& name);

		bool is_valid() const noexcept;
		bool notify(
				std::string const& email,
				std::string const& name,
				std::string const message) noexcept;

		SMTP::server const& server() const noexcept;
		void server(SMTP::server const&) noexcept;

		std::string const& name() const noexcept;

		static bool is_valid(SMTP::server const&) noexcept;
	private:
		boost::asio::io_context& ioc_;
		boost::asio::ssl::context ctx_{boost::asio::ssl::context::sslv23};
		SMTP::server server_;
		std::string name_;
};


#endif /* AGRO_DAEMON_NOTIFY_MAIL_HPP__ */
