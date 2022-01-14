#include "list.hpp"
#include <algorithm>

namespace Agro{
namespace User{

User* User_List::add(User&& user) noexcept
{
	auto u = list_.find(user.id());
	if(u != list_.end())
	{
		return nullptr;
	}

	return &list_.emplace(user.id(), user).first->second;
}

bool User_List::remove(user_id id) noexcept
{
	auto u = list_.find(id);
	if(u == list_.end())
	{
		return false;
	}
	list_.erase(id);

	return true;
}

User const* User_List::get(user_id id) const noexcept
{
	for(auto const& [uid, user] : list_)
	{
		if(uid == id)
			return &user;
	}
	return nullptr;
}

User* User_List::get(user_id id) noexcept
{
	for(auto & [uid, user] : list_)
	{
		if(uid == id)
			return &user;
	}
	return nullptr;
}

User const* User_List::get(std::string const& username) const noexcept
{
	for(auto const& [uid, user] : list_)
	{
		if(user.info().username() == username)
			return &user;
	}
	return nullptr;
}

User* User_List::get(std::string const& username) noexcept
{
	for(auto& [uid, user] : list_)
	{
		if(user.info().username() == username)
			return &user;
	}
	return nullptr;
}

User const* User_List::operator[](user_id id) const noexcept
{
	return get(id);
}

User* User_List::operator[](user_id id) noexcept
{
	return get(id);
}

User const* User_List::operator[](std::string const& username) const noexcept
{
	return get(username);
}

User* User_List::operator[](std::string const& username) noexcept
{
	return get(username);
}

//Groups& User_List::groups() noexcept
//{
//	return groups_;
//}

Groups const& User_List::groups() const noexcept
{
	return groups_;
}

bool User_List::add_group(Group&& group) noexcept
{
	bool ret = groups_.add(std::move(group));
	if(ret)
	{
		update_user_policy();
	}
	return groups_.add(std::move(group));
}

bool User_List::remove_group(group_id gid) noexcept
{
	bool ret = groups_.remove(gid);
	if(ret)
	{
		update_user_policy();
	}

	return ret;
}

Authorization::Policies const& User_List::policies() const noexcept
{
	return policy_rules_;
}

Authorization::Policies& User_List::policies() noexcept
{
	return policy_rules_;
}

bool User_List::add_user_to_group(user_id uid, group_id gid) noexcept
{
	bool ret = groups_.add_user(gid, uid);
	if(ret)
		update_user_policy(uid);
	return ret;
}

void User_List::add_user_to_groups(user_id uid, std::vector<group_id> const& gid_list) noexcept
{
	groups_.add_user_to_groups(uid, gid_list);
	update_user_policy(uid);
}

void User_List::remove_user_from_all_groups(user_id uid) noexcept
{
	groups_.remove_user_from_all(uid);
}

void User_List::set_user_to_groups(user_id uid, std::vector<group_id> const& gid_list) noexcept
{
	groups_.remove_user_from_all(uid);
	groups_.add_user_to_groups(uid, gid_list);
}

bool User_List::edit_user(user_id id,
		std::string const& username,
		std::string const& name,
		std::string const& email,
		std::string const& telegram_chat_id,
		std::vector<group_id> const& groups) noexcept
{
	auto* user = get(id);
	if(!user) return false;

	user->info().set(username, name, user->info().get_status(), email, telegram_chat_id);

	groups_.remove_user_from_all(id);
	groups_.add_user_to_groups(id, groups);

	update_user_policy(*user);

	return true;
}

void User_List::update_user_policy(user_id id) noexcept
{
	auto* user = get(id);
	if(!user) return;

	user->policy(static_cast<int>(policy_rules_.get_policy(id, groups_)));
}

void User_List::update_user_policy(User& user) noexcept
{
	user.policy(static_cast<int>(policy_rules_.get_policy(user.id(), groups_)));
}

void User_List::update_user_policy() noexcept
{
	for(auto& [uid, u] : list_)
	{
		update_user_policy(u);
	}
}

}//User
}//Agro
