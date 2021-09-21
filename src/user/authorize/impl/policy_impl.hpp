#ifndef AGRO_DAEMON_USER_AUTH_POLICY_IMPL_HPP__
#define AGRO_DAEMON_USER_AUTH_POLICY_IMPL_HPP__

#include "../policy.hpp"
#include "../../helper/enum_ops.hpp"

namespace Agro{
namespace Authorization{

template<typename Group>
Permission_Group<Group>::Permission_Group(
		perm_id id,
		type tp,
		action act,
		Group const& group,
		effect eft/* = effect::allow */)
	: Permission(id, tp, act, eft), group_(group){}

template<typename Group>
effect Permission_Group<Group>::can(type tp, action act, ref_id rid) const noexcept
{
	if(type_ != tp) return effect::not_allow;
	if(!(action_ & act)) return effect::not_allow;
	if(group_.contains(rid)) return eft_;

	return effect::not_allow;
}

template<typename Group>
ref_type Permission_Group<Group>::reference_type() const noexcept
{
	return ref_type::group;
}

template<typename Group>
ref_id Permission_Group<Group>::reference_id() const noexcept
{
	return group_.id();
}

}//Authorization
}//Agro

#endif /* AGRO_DAEMON_USER_AUTH_POLICY_IMPL_HPP__ */
