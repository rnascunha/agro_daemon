#include "policy.hpp"
#include <algorithm>

#include "../helper/enum_ops.hpp"

namespace Agro{
namespace Authorization{

Permission::Permission(perm_id id, type tp, action act, effect eft)
	: id_(id), type_(tp), action_(act), eft_(eft){}

perm_id Permission::id() const noexcept
{
	return id_;
}

type Permission::get_type() const noexcept
{
	return type_;
}

action Permission::get_action() const noexcept
{
	return action_;
}

effect Permission::get_effect() const noexcept
{
	return eft_;
}

Permission_Single::Permission_Single(perm_id id,
		type ptype,
		action act,
		ref_id rid /* = permission_any */,
		effect eft /* = effect::allow */)
	: Permission{id, ptype, act, eft},
	  ref_id_(rid){}

effect Permission_Single::can(type tp, action act, ref_id rid) const noexcept
{
	if(type_ != tp) return effect::not_allow;
	if(!(action_ & act)) return effect::not_allow;
	if(ref_id_ == permission_any || ref_id_ == rid) return eft_;

	return effect::not_allow;
}

ref_type Permission_Single::reference_type() const noexcept
{
	return ref_type::single;
}

ref_id Permission_Single::reference_id() const noexcept
{
	return ref_id_;
}

/**
 *
 */
//Permission_Group::Permission_Group(perm_id id,
//		type ptype,
//		action act,
//		effect eft/* = effect::allow */)
//	: Permission{id, ptype, act, eft}
//{}
//
//bool Permission_Group::add(ref_id id) noexcept
//{
//	auto iter = std::find(ref_ids_.begin(), ref_ids_.end(), id);
//	if(iter == ref_ids_.end())
//	{
//		return false;
//	}
//
//	ref_ids_.push_back(id);
//	return true;
//}
//
//effect Permission_Group::can(type tp, action act, ref_id rid) const noexcept
//{
//	if(type_ != tp) return effect::not_allow;
//	if(!(action_ & act)) return effect::not_allow;
//
//	for(auto const& i : ref_ids_)
//	{
//		if(i == permission_any || i == rid)
//		{
//			return eft_;
//		}
//	}
//
//	return effect::not_allow;
//}

/**
 *
 */
bool Permission_List::add(std::shared_ptr<Permission> perm) noexcept
{
	for(auto&& p : perms_)
	{
		if(p->id() == perm->id())
		{
			return false;
		}
	}

	perms_.emplace_back(perm);
	return true;
}

bool Permission_List::remove_all(type tp, ref_type rtype, ref_id rid) noexcept
{
    auto iter_end = std::remove_if(perms_.begin(), perms_.end(),
    		[&tp, &rtype, rid](std::shared_ptr<Permission> perm){
    	return perm->get_type() == tp
    			&& perm->reference_type() == rtype
				&& perm->reference_id() == rid;
    });

    perms_.erase(iter_end, perms_.end());

    return true;
}

std::shared_ptr<Permission> Permission_List::get(perm_id id) noexcept
{
	for(auto&& p : perms_)
	{
		if(p->id() == id)
			return p;
	}
	return std::shared_ptr<Permission>{};
}

/**
 *
 */
Policy::Policy(policy_id id, std::shared_ptr<Permission const> perm)
	: id_(id), perm_(perm){}

policy_id Policy::id() const noexcept
{
	return id_;
}

/**
 *
 */
Policy_Single::Policy_Single(policy_id pid, role_id rid, std::shared_ptr<Permission const> perm)
	: Policy(pid, perm), role_id_(rid){}

effect Policy_Single::can(role_id id, type tp, action act, ref_id rfid) const noexcept
{
	if(role_id_ != id) return effect::not_allow;
	return perm_->can(tp, act, rfid);
}

Policy_Group::Policy_Group(policy_id pid,
		User::Group const& group,
		std::shared_ptr<Permission const> perm)
	: Policy(pid, perm), group_(group){}

effect Policy_Group::can(role_id id, type tp, action act, ref_id rfid) const noexcept
{
	if(!group_.contains(id)) return effect::not_allow;
	return perm_->can(tp, act, rfid);
}

bool Enforce::add_policy(std::unique_ptr<Policy> policy) noexcept
{
	for(auto&& p : policies_)
	{
		if(p->id() == policy->id())
			return false;
	}

	policies_.push_back(std::move(policy));
	return true;
}

bool Enforce::remove_policy(policy_id id) noexcept
{
	int i = 0;
	for(auto&& p : policies_)
	{
		if(p->id() == id)
		{
			policies_.erase(policies_.begin() + i);
			return true;
		}
		i++;
	}
	return false;
}

bool Enforce::can(role_id id, type tp, action act, ref_id rfid) const noexcept
{
	for(auto const& policy : policies_)
	{
		if(policy->can(id, tp, act, rfid) == effect::allow)
		{
			return true;
		}
	}
	return false;
}

}//Authorization
}//Agro
