#include "agro.hpp"

namespace Agro{

bool instance::notify_is_valid() const noexcept
{
	return notify_.is_valid();
}

std::string_view const& instance::get_notify_public_key() const noexcept
{
	return notify_.public_key();
}

void instance::push_subscribe_user(User::user_id id,
			std::string const& user_agent,
			std::string const& endpoint,
			std::string const& p256dh,
			std::string const& auth) noexcept
{
	auto* user = users_[id];
	if(!user) return;

	user->subscriptions().add_or_update(user_agent, endpoint, p256dh, auth);
	db_.push_subscribe_user(id, user_agent, endpoint, p256dh, auth);
}

void instance::push_unsubscribe_user(User::user_id id,
			std::string const& user_agent) noexcept
{
	auto* user = users_[id];
	if(!user) return;

	user->subscriptions().remove(user_agent);
	db_.push_unsubscribe_user(id, user_agent);
}

void instance::notify_all(std::string const& data) noexcept
{
	for(auto const& [uid, u] : users_)
	{
		for(auto const& s : u.subscriptions() )
			notify_.notify(s, data);
	}
}

void instance::notify_all_policy(Authorization::rule rule, std::string const& data) noexcept
{
	for(auto const& [uid, u] : users_)
	{
		if(Authorization::can(u, rule))
		{
			for(auto const& s : u.subscriptions() )
				notify_.notify(s, data);
		}
	}
}

}//Agro
