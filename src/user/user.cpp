#include "user.hpp"
#include <algorithm>
#include "../helper/time_helper.hpp"

namespace Agro{
namespace User{

/**
 *
 *
 */
Subscription::Subscription(std::string const user_agent,
		std::string const& endpoint,
		std::string const& p256dh,
		std::string const auth)
	: user_agent_(user_agent), endpoint_(endpoint),
	  p256dh_(p256dh), auth_(auth){}

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
void Subscription_List::add_or_update(
		std::string const& user_agent,
		std::string const& endpoint,
		std::string const& p256dh,
		std::string const& auth) noexcept
{
	for(auto& l : list_)
	{
		if(l.user_agent() == user_agent)
		{
			l.update(endpoint, p256dh, auth);
			return;
		}
	}
	list_.emplace_back(user_agent, endpoint, p256dh, auth);
}

bool Subscription_List::remove(std::string const& user_agent) noexcept
{
	int i = 0;
	for(auto& l : list_)
	{
		if(l.user_agent() == user_agent)
		{
			list_.erase(list_.begin() + i);
			return true;
		}
		i++;
	}
	return false;
}

void Subscription_List::clear_subscription(std::string const& user_agent) noexcept
{
    auto iter_end = std::remove_if(list_.begin(), list_.end(),
    		[&user_agent](Subscription const& sub){
    	return sub.user_agent() == user_agent;
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
Session::Session(std::string const& user_agent,
		std::string const& session_id,
		long session_time)
	:  user_agent_(user_agent),
	  session_id_(session_id),
	  session_time_(session_time){}

Session::Session(std::string const& user_agent,
				std::string const& session_id)
	: user_agent_(user_agent),
	 session_id_(session_id),
	 session_time_(time_epoch_seconds()){}

std::string const& Session::user_agent() const noexcept
{
	return user_agent_;
}

std::string const& Session::session_id() const noexcept
{
	return session_id_;
}

bool Session::check(std::string const& session_id,
				std::string const& user_agent) const noexcept
{
	return session_id_ == session_id &&
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
		if(l.user_agent() == session.user_agent())
		{
			l.update(session.session_id());
			return;
		}
	}
	list_.emplace_back(session);
}

void Session_List::add_or_update(std::string const& session_id,
				std::string const& user_agent) noexcept
{
	for(auto& l : list_)
	{
		if(l.user_agent() == user_agent)
		{
			l.update(session_id);
			return;
		}
	}
	list_.emplace_back(session_id, user_agent);
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
				std::string const& session_id,
				std::string const& user_agent,
				long& session_time) const noexcept
{
	session_time = 0;
	for(auto const& l : list_)
	{
		if(l.check(session_id, user_agent))
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
void Logged::user(User* user) noexcept
{
	user_ = user;
	id_ = user->id();
}

user_id Logged::id() const noexcept
{
	return id_;
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

int Logged::policy_rules() const noexcept
{
	return user_ == nullptr ? 0 : user_->policy();
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

User* Logged::user() noexcept
{
	return user_;
}

User const* Logged::user() const noexcept
{
	return user_;
}

/***
 *
 *
 *
 *
 */
User::User(user_id id, Info&& info)
	: id_(id), info_(info){}

user_id	User::id() const noexcept
{
	return id_;
}

int User::policy() const noexcept
{
	return policy_;
}

void User::policy(int pol) noexcept
{
	policy_ = pol;
}

Info const& User::info() const noexcept
{
	return info_;
}
Info& User::info() noexcept
{
	return info_;
}

Subscription_List const& User::subscriptions() const noexcept
{
	return subscriptions_;
}

Subscription_List& User::subscriptions() noexcept
{
	return subscriptions_;
}

Session_List const& User::sessions() const noexcept
{
	return sessions_;
}

Session_List& User::sessions() noexcept
{
	return sessions_;
}

Notify::Notify const& User::notify() const noexcept
{
	return notify_;
}

Notify::Notify& User::notify() noexcept
{
	return notify_;
}

}//User
}//Agro
