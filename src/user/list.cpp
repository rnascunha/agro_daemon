#include "list.hpp"
#include <algorithm>

namespace Agro{
namespace User{

Info_List& Users::infos() noexcept
{
	return user_list_;
}

Info_List const& Users::infos() const noexcept
{
	return user_list_;
}

bool Users::add_user(Info&& user) noexcept
{
	return user_list_.add(std::move(user));
}

void Users::add_user_to_groups(user_id uid, std::vector<group_id> const& gid_list) noexcept
{
	groups_.add_user_to_groups(uid, gid_list);
}

bool Users::edit_user(user_id id,
				std::string const& username,
				std::string const& name,
				std::string const& email,
				std::vector<group_id> const& groups) noexcept
{
	if(!user_list_.update(id, username, name, email))
	{
		return false;
	}

	groups_.remove_user_from_all(id);
	groups_.add_user_to_groups(id, groups);

	return true;
}

Subscription_List& Users::subscriptions() noexcept
{
	return sub_list_;
}

Subscription_List const& Users::subscriptions() const noexcept
{
	return sub_list_;
}

Session_List& Users::sessions() noexcept
{
	return session_list_;
}

Session_List const& Users::sessions() const noexcept
{
	return session_list_;
}

Groups& Users::groups() noexcept
{
	return groups_;
}

Groups const& Users::groups() const noexcept
{
	return groups_;
}

}//User
}//Agro
