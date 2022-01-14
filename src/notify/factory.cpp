#include "factory.hpp"

namespace Agro{
namespace Notify{

bool notify_impl::enable() const noexcept
{
	return enable_;
}

void notify_impl::enable(bool en) noexcept
{
	enable_ = en;
}

push::push(boost::asio::io_context& ioc,
				pusha::key&& key,
				std::string_view const& subscriber)
	: push_{ioc, std::move(key), subscriber}{}

void push::notify(User::User const& user, std::string const& message) noexcept
{
	for(auto const& s : user.subscriptions())
	{
		push_.notify(s, message);
	}
}

std::string_view const& push::public_key() const noexcept
{
	return push_.public_key();
}

bool push::is_valid() const noexcept
{
	return push_.is_valid();
}

/**
 *
 */

telegram::telegram(boost::asio::io_context& ioc, std::string const& token)
	: tele_{ioc, token}{}

void telegram::notify(User::User const& user, std::string const& message) noexcept
{
	tele_.notify(user.info().telegram_chat_id(), message);
}

bool telegram::is_valid() const noexcept
{
	return tele_.is_valid();
}

std::string const& telegram::token() const noexcept
{
	return tele_.token();
}

void telegram::token(std::string const& ntoken) noexcept
{
	tele_.token(ntoken);
}

/**
 *
 */

mail::mail(boost::asio::io_context& ioc,
			SMTP::server const& server,
			std::string const& name)
	: mail_(ioc, server, name){}

void mail::notify(User::User const& user, std::string const& message) noexcept
{
	mail_.notify(user.info().email(), user.info().name(), message);
}

bool mail::is_valid() const noexcept
{
	return mail_.is_valid();
}

SMTP::server const& mail::server() const noexcept
{
	return mail_.server();
}

void mail::server(SMTP::server const& server) noexcept
{
	mail_.server(server);
}

std::string const& mail::name() const noexcept
{
	return mail_.name();
}

/**
 *
 */

Factory::Factory(){}

bool Factory::push_is_valid() const noexcept
{
	auto it = notifys_.find("push");
	if(it == notifys_.end())
	{
		return false;
	}

	return static_cast<push*>(it->second.get())->is_valid();
}

std::string_view const& Factory::public_key() const noexcept
{
	static constexpr const std::string_view vw{""};
	auto it = notifys_.find("push");
	if(it == notifys_.end())
	{
		return vw;
	}

	return static_cast<push*>(it->second.get())->public_key();
}

bool Factory::add(std::string const& name, std::unique_ptr<notify_impl> notify) noexcept
{
	notifys_.emplace(name, std::move(notify));

	return true;
}

void Factory::notify(User::User const& user, std::string const& message) noexcept
{
	for(auto& [name, n] : notifys_)
	{
		if(n->enable() && n->is_valid())
		{
			n->notify(user, message);
		}
	}
}

notify_impl* Factory::get(std::string const& name) noexcept
{
	auto it = notifys_.find(name);
	if(it == notifys_.end())
	{
		return nullptr;
	}

	return it->second.get();
}

}//Notify
}//Agro
