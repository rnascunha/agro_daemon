#ifndef AGRO_DAEMON_USER_AUTH_POLICY_HPP__
#define AGRO_DAEMON_USER_AUTH_POLICY_HPP__

#include <vector>
#include <unordered_map>
#include <memory>
#include "group.hpp"

namespace Agro{
namespace Authorization{

static constexpr const int permission_any = -1;

using perm_id = int;
using ref_id = int;
using role_id = int;
using policy_id = int;

template<typename Enum>
struct name_value{
	Enum		value;
	const char* name;
};

enum class type{
	user = 0,
	user_group,
	device,
	net,
	command,
	resource,
	image,
	app
};

static constexpr const name_value<type> type_name[] = {
		{type::user, "user"},
		{type::user_group, "user_group"},
		{type::device, "device"},
		{type::net, "net"},
		{type::command, "command"},
		{type::resource, "resource"},
		{type::image, "image"},
		{type::app, "app"}
};

enum class action{
	create = 1 << 0,
	view = 1 << 1,
	execute = 1 << 2,
	adelete = 1 << 3,
	add = 1 << 4,
	remove = 1 << 5,
	all = create | view | execute |
			adelete | add | remove
};

static constexpr const name_value<action> action_name[] = {
		{action::create, "create"},
		{action::view, "view"},
		{action::execute, "execute"},
		{action::adelete, "delete"},
		{action::add, "add"},
		{action::remove, "remove"},
		{action::all, "all"},
};

enum class effect{
	deny = 0,
	allow,
	not_allow,
};

static constexpr const name_value<effect> effect_name[] = {
		{effect::deny, "create"},
		{effect::allow, "view"},
		{effect::not_allow, "not_allow"}
};

enum class ref_type{
	single = 0,
	group
};

/**
 *
 */
class Permission{
	public:
		Permission(perm_id, type, action, effect);

		virtual effect can(type, action, ref_id) const noexcept = 0;
		virtual ref_type reference_type() const noexcept = 0;
		virtual ref_id reference_id() const noexcept = 0;
		virtual ~Permission(){};

		perm_id id() const noexcept;
		type get_type() const noexcept;
		action get_action() const noexcept;
		effect get_effect() const noexcept;
	protected:
		perm_id		id_;
		type		type_;
		action		action_;
		effect		eft_;
};

/**
 *
 */
class Permission_Single : public Permission{
	public:
		Permission_Single(perm_id,
				type,
				action,
				ref_id = permission_any,
				effect = effect::allow);

		effect can(type, action, ref_id) const noexcept override;
		ref_type reference_type() const noexcept override;
		ref_id reference_id() const noexcept override;
	private:
		ref_id		ref_id_;
};

/**
 *
 */
//class Permission_Group : public Permission{
//	public:
//		Permission_Group(perm_id,
//				type,
//				action,
//				effect = effect::allow);
//
//		bool add(ref_id) noexcept;
//		effect can(type, action, ref_id) const noexcept override;
//	private:
//		std::vector<ref_id>	ref_ids_;
//};

/**
 *
 */
template<typename Group>
class Permission_Group : public Permission{
	public:
		Permission_Group(perm_id,
				type,
				action,
				Group const&,
				effect = effect::allow);

		effect can(type, action, ref_id) const noexcept override;
		ref_type reference_type() const noexcept override;
		ref_id reference_id() const noexcept override;
	private:
		Group const& group_;
};

/**
 *
 */
class Permission_List{
	public:
		bool add(std::shared_ptr<Permission> perm) noexcept;
		bool remove_all(type, ref_type, ref_id) noexcept;
		std::shared_ptr<Permission> get(perm_id) noexcept;

		std::vector<std::shared_ptr<Permission>>::const_iterator begin() const { return perms_.begin(); }
		std::vector<std::shared_ptr<Permission>>::const_iterator end() const { return perms_.end(); }
		std::vector<std::shared_ptr<Permission>>::const_iterator cbegin() const { return perms_.cbegin(); }
		std::vector<std::shared_ptr<Permission>>::const_iterator cend() const { return perms_.cend(); }
	private:
		std::vector<std::shared_ptr<Permission>> perms_;
};


class Policy{
	public:
		Policy(policy_id, std::shared_ptr<Permission const>);
		policy_id id() const noexcept;
		virtual effect can(role_id, type, action, ref_id) const noexcept = 0;
		virtual ~Policy(){}
	protected:
		policy_id	id_;
		std::shared_ptr<Permission const> perm_;
};

/**
 *
 */
class Policy_Single : public Policy{
	public:
		Policy_Single(policy_id, role_id, std::shared_ptr<Permission const>);
		effect can(role_id, type, action, ref_id) const noexcept override;
	private:
		role_id	role_id_;
};

/**
 *
 */
class Policy_Group : public Policy{
	public:
		Policy_Group(policy_id, User::Group const& group, std::shared_ptr<Permission const>);
		effect can(role_id, type, action, ref_id) const noexcept override;
	private:
		User::Group const& group_;
};

/**
 *
 */
class Enforce{
	public:
		bool add_policy(std::unique_ptr<Policy> policy) noexcept;
		bool remove_policy(policy_id id) noexcept;

		bool can(role_id, type, action, ref_id) const noexcept;
	private:
		std::vector<std::unique_ptr<Policy>> policies_;
};

}//Authorization
}//Agro

#include "impl/policy_impl.hpp"

#endif /* AGRO_DAEMON_USER_AUTH_POLICY_HPP__ */
