#include "user.hpp"

namespace Agro{

User::User(){}

User::User(int id, std::string const& username, std::string const& name,
		status stat, std::string const& email)
	: id_(id), username_(username), name_(name),
	  status_(stat), email_(email){}

User::User(int id, const char* username, const char* name,
		status stat, const char* email)
	: id_(id), username_(username), name_(name ? name : ""),
	  status_(stat), email_(email ? email : ""){}


void User::set(int id, std::string const& username, std::string const& name,
		status stat, std::string const& email) noexcept
{
	id_ = id;
	username_ = username;
	name_ = name;
	status_ = stat;
	email_ = email;
}

bool User::is_valid() const noexcept
{
	return status_ != status::invalid;
}

bool User::is_authenticated() const noexcept
{
	return authenticated_;
}

int User::id() const noexcept
{
	return id_;
}

std::string const& User::username() const noexcept
{
	return username_;
}

std::string const& User::name() const noexcept
{
	return name_;
}

User::status	User::get_status() const noexcept
{
	return status_;
}

std::string const& User::email() const noexcept
{
	return email_;
}

std::string const& User::session_id() const noexcept
{
	return session_id_;
}

std::string const& User::user_agent() const noexcept
{
	return user_agent_;
}

void User::authenticate() noexcept
{
	authenticated_ = true;
}

void User::authenticate(std::string const& session_id,
				std::string const& user_agent) noexcept
{
	authenticated_ = true;
	session_id_ = session_id;
	user_agent_ = user_agent;
}

User::operator bool() const noexcept
{
	return is_valid();
}

bool User::is_subscribed() const noexcept
{
	return is_subscribed_;
}

void User::push_subscribe(std::string const& endpoint,
		std::string const& p256dh,
		std::string const& auth) noexcept
{
	endpoint_ = endpoint;
	p256dh_ = p256dh;
	auth_ = auth;
	is_subscribed_ = true;
}

void User::push_unsubscribe() noexcept
{
	endpoint_.clear();
	p256dh_.clear();
	auth_.clear();
	is_subscribed_ = false;
}

std::string const& User::endpoint() const noexcept
{
	return endpoint_;
}
std::string const& User::p256dh() const noexcept
{
	return p256dh_;
}

std::string const& User::auth() const noexcept
{
	return auth_;
}

bool User::to_alert(alert alrt) const noexcept
{
	return alert_ & alrt;
}

}//Agro
