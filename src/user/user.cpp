#include "user.hpp"
#include <algorithm>
#include "../helper/time_helper.hpp"

namespace Agro{
namespace User{

Info::Info(){}

Info::Info(int id, std::string const& username, std::string const& name,
		status stat, std::string const& email)
	: id_(id), username_(username), name_(name),
	  status_(stat), email_(email){}

Info::Info(int id, const char* username, const char* name,
		status stat, const char* email)
	: id_(id), username_(username), name_(name ? name : ""),
	  status_(stat), email_(email ? email : ""){}


void Info::set(int id, std::string const& username, std::string const& name,
		status stat, std::string const& email) noexcept
{
	id_ = id;
	username_ = username;
	name_ = name;
	status_ = stat;
	email_ = email;
}

bool Info::is_valid() const noexcept
{
	return status_ != status::invalid;
}

int Info::id() const noexcept
{
	return id_;
}

std::string const& Info::username() const noexcept
{
	return username_;
}

std::string const& Info::name() const noexcept
{
	return name_;
}

Info::status	Info::get_status() const noexcept
{
	return status_;
}

std::string const& Info::email() const noexcept
{
	return email_;
}

/**
 *
 *
 */
bool Info_List::add(Info&& info) noexcept
{
	for(auto const& l : list_)
		if(l.id() == info.id()) return false;

	list_.push_back(info);
	return true;
}

bool Info_List::remove(user_id id) noexcept
{
	int i = 0;
	for(auto& l : list_)
	{
		if(l.id() == id)
		{
			list_.erase(list_.begin() + i);
			return true;
		}
		i++;
	}
	return false;
}

user_id Info_List::get_id(std::string const& username) const noexcept
{
	for(auto const& l : list_)
	{
		if(l.username() == username)
			return l.id();
	}
	return invalid_id;
}

Info const* Info_List::get(std::string const& username) const noexcept
{
	for(auto& l : list_)
	{
		if(l.username() == username)
			return &l;
	}
	return nullptr;
}

std::size_t Info_List::size() const noexcept
{
	return list_.size();
}

/**
 *
 *
 */
Subscription::Subscription(user_id id, std::string const user_agent,
		std::string const& endpoint,
		std::string const& p256dh,
		std::string const auth)
	: id_(id), user_agent_(user_agent), endpoint_(endpoint),
	  p256dh_(p256dh), auth_(auth){}

user_id Subscription::id() const noexcept
{
	return id_;
}

std::string const& Subscription::user_agent() const noexcept
{
	return user_agent_;
}

std::string const& Subscription::endpoint() const noexcept
{
	return endpoint_;
}

std::string const& Subscription::p256dh() const noexcept
{
	return p256dh_;
}

std::string const& Subscription::auth() const noexcept
{
	return auth_;
}

void Subscription::update(std::string const& endpoint,
				std::string const& p256dh,
				std::string const auth) noexcept
{
	endpoint_ = endpoint;
	p256dh_ = p256dh;
	auth_ = auth;
}

/**
 *
 */
void Subscription_List::add_or_update(user_id id,
		std::string const& user_agent,
		std::string const& endpoint,
		std::string const& p256dh,
		std::string const& auth) noexcept
{
	for(auto& l : list_)
	{
		if(l.id() == id && l.user_agent() == user_agent)
		{
			l.update(endpoint, p256dh, auth);
			return;
		}
	}
	list_.emplace_back(id, user_agent, endpoint, p256dh, auth);
}

bool Subscription_List::remove(user_id id, std::string const& user_agent) noexcept
{
	int i = 0;
	for(auto& l : list_)
	{
		if(l.id() == id && l.user_agent() == user_agent)
		{
			list_.erase(list_.begin() + i);
			return true;
		}
		i++;
	}
	return false;
}

void Subscription_List::clear_subscription(user_id id, std::string const& user_agent) noexcept
{
    auto iter_end = std::remove_if(list_.begin(), list_.end(),
    		[&id, &user_agent](Subscription const& sub){
    	return sub.id() == id && sub.user_agent() == user_agent;
    });

    list_.erase(iter_end, list_.end());
}

std::size_t Subscription_List::size() const noexcept
{
	return list_.size();
}

/**
 *
 *
 */
Session::Session(user_id id,
		std::string const& user_agent,
		std::string const& session_id,
		long session_time)
	: id_(id),
	  user_agent_(user_agent),
	  session_id_(session_id),
	  session_time_(session_time){}

Session::Session(user_id id,
				std::string const& user_agent,
				std::string const& session_id)
	: id_(id),
	 user_agent_(user_agent),
	 session_id_(session_id),
	 session_time_(time_epoch_seconds()){}

user_id Session::id() const noexcept
{
	return id_;
}

std::string const& Session::user_agent() const noexcept
{
	return user_agent_;
}

std::string const& Session::session_id() const noexcept
{
	return session_id_;
}

bool Session::check(User::user_id id,
				std::string const& session_id,
				std::string const& user_agent) const noexcept
{
	return id_ == id &&
			session_id_ == session_id &&
			user_agent_ == user_agent;
}

bool Session::check_time(long session_time) const noexcept
{
	return (session_time_ + session_time) < time_epoch_seconds();
}

long Session::session_time() const noexcept
{
	return session_time_;
}

long Session::login_time() const noexcept
{
	return session_time_login_;
}

long Session::logout_time() const noexcept
{
	return session_time_logout_;
}

bool Session::is_logged() const noexcept
{
	return logged_;
}

void Session::login(std::string const& session_id) noexcept
{
	session_id_ = session_id;
	session_time_login_ = time_epoch_seconds();
	logged_ = true;
}

void Session::logout() noexcept
{
	session_time_logout_ = time_epoch_seconds();
	logged_ = false;
}

void Session::update(std::string const& session_id) noexcept
{
	session_id_ = session_id;
}

/**
 *
 */
void Session_List::add_or_update(Session&& session) noexcept
{
	for(auto& l : list_)
	{
		if(l.id() == session.id() && l.user_agent() == session.user_agent())
		{
			l.update(session.session_id());
			return;
		}
	}
	list_.emplace_back(session);
}

void Session_List::add_or_update(user_id id,
				std::string const& session_id,
				std::string const& user_agent) noexcept
{
	for(auto& l : list_)
	{
		if(l.id() == id && l.user_agent() == user_agent)
		{
			l.update(session_id);
			return;
		}
	}
	list_.emplace_back(id, session_id, user_agent);
}

bool Session_List::remove(std::string const& session_id) noexcept
{
	int i = 0;
	for(auto& l : list_)
	{
		if(l.session_id() == session_id)
		{
			list_.erase(list_.begin() + i);
			return true;
		}
		i++;
	}
	return false;
}

bool Session_List::check_user_session_id(
				User::user_id id,
				std::string const& session_id,
				std::string const& user_agent,
				long& session_time) const noexcept
{
	session_time = 0;
	for(auto const& l : list_)
	{
		if(l.check(id, session_id, user_agent))
		{
			session_time = l.session_time();
			return true;
		}
	}

	return false;
}

std::size_t Session_List::size() const noexcept
{
	return list_.size();
}

/**
 *
 */
void Logged::info(Info const* info) noexcept
{
	info_ = info;
}

std::string const& Logged::user_agent() const noexcept
{
	return user_agent_;
}

void Logged::user_agent(std::string const& uagent) noexcept
{
	user_agent_ = uagent;
}

std::string const& Logged::session_id() const noexcept
{
	return session_id_;
}

bool Logged::is_authenticated() const noexcept
{
	return authenticated_;
}

void Logged::authenticate() noexcept
{
	authenticated_ = true;
}

void Logged::authenticate(std::string const& session_id,
		std::string const& user_agent) noexcept
{
	session_id_ = session_id;
	user_agent_ = user_agent;
	authenticated_ = true;
}

Info const* Logged::info() const noexcept
{
	return info_;
}

}//User
}//Agro
