#include "policy.hpp"
#include "../helper/enum_ops.hpp"
#include <algorithm>
#include "user.hpp"

namespace Agro{
namespace Authorization{

rule Policies::get_policy(User::user_id uid, User::Group const& group) const noexcept
{
	rule rules = rule::none;
	for(auto const& p : policies_)
	{
		if(group.id() == p.gid
			&& group.contains(uid))
		{
			rules |= p.rules;
		}
	}
	return rules;
}

rule Policies::get_policy(User::user_id uid, User::Groups const& group_list) const noexcept
{
	rule rules = rule::none;
	for(auto const& g : group_list)
	{
		rules |= get_policy(uid, g);
	}

	return rules;
}

bool Policies::add(policy_id pid, User::group_id gid, rule rule) noexcept
{
	for(auto const& p : policies_)
	{
		if(pid == p.id)
			return false;
	}
	policies_.emplace_back(pid, gid, rule);

	return true;
}

void Policies::remove_group(User::group_id gid) noexcept
{
	auto iter_end = std::remove_if(policies_.begin(), policies_.end(),
	    		[&gid](Policy const& policy){
		return policy.gid == gid;
	});

	policies_.erase(iter_end, policies_.end());
}

bool can(User::Logged const& user, rule rule) noexcept
{
	if(user.id() == User::root_id) return true;

	return user.policy_rules() & static_cast<int>(rule);
}

bool can(User::User const& user, rule rule) noexcept
{
	if(user.id() == User::root_id) return true;

	return user.policy() & static_cast<int>(rule);
}

}//Authorization
}//Agro
