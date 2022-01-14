#include "mail.hpp"

using SMPT_Client = SMTP::Client<false, 10>;

mail_factory::mail_factory(boost::asio::io_context& ioc,
		SMTP::server const& server,
		std::string const& name)
	: ioc_(ioc), server_(server), name_(name)
{}

bool mail_factory::is_valid() const noexcept
{
	return is_valid(server_);
}

bool mail_factory::is_valid(SMTP::server const& server) noexcept
{
	return !server.server.empty()
			&& !server.port.empty()
			&& !server.user.empty()
			&& !server.password.empty();
}

bool mail_factory::notify(
		std::string const& email,
		std::string const& name,
		std::string const message) noexcept
{
	if(email.empty())
	{
		return false;
	}

	auto mail = std::make_shared<SMPT_Client>(ioc_, ctx_);

	SMTP::data data;
	SMTP::make_mail_data(data,
			{server_.user, name_},
			{{email, name}},
			"Agro Notify", message);

	mail->set_mail_server(server_);
	mail->set_mail_data(data);

	mail->connect();

	return true;
}

SMTP::server const& mail_factory::server() const noexcept
{
	return server_;
}

void mail_factory::server(SMTP::server const& server) noexcept
{
	server_ = server;
}

std::string const& mail_factory::name() const noexcept
{
	return name_;
}
