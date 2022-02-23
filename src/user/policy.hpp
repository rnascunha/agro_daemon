#ifndef AGRO_DAEMON_USER_AUTH_POLICY_HPP__
#define AGRO_DAEMON_USER_AUTH_POLICY_HPP__

#include <limits>
#include "group.hpp"

namespace Agro{
namespace User{

class Logged;
class User;

}//User
}//Agro

namespace Agro{
namespace Authorization{

static constexpr const int permission_any = -1;

using policy_type_id = int;
using policy_id = int;

enum class rule{
	none = 0,
	user_admin = 1 << 0,
	view_device = 1 << 1,
	edit_device = 1 << 2,
	all_device = view_device | edit_device,
	get_resource = 1 << 3,
	post_resource = 1 << 4,
	put_resource = 1 << 5,
	delete_resource = 1 << 6,
	all_resources = get_resource | put_resource | post_resource | delete_resource,
	view_image = 1 << 7,
	upload_image = 1 << 8,
	install_image = 1 << 9,
	all_image = view_image | upload_image | install_image,
	view_app = 1 << 10,
	upload_app = 1 << 11,
	install_app = 1 << 12,
	all_app = view_app | upload_app | install_app,
	all = std::numeric_limits<int>::max()
};

struct Policy_Type{
	Policy_Type(policy_type_id iid, rule rl, std::string nm, std::string desc)
		: id(iid), code(rl), name(nm), description(desc){}

	policy_type_id 	id;
	rule			code;
	std::string		name;
	std::string 	description;
};

using Policy_Types = std::vector<Policy_Type>;

struct Policy{
	Policy(policy_id iid, User::group_id ggid, rule rl)
		: id(iid), gid(ggid), rules(rl){}

	policy_id		id;
	User::group_id 	gid;
	rule			rules = rule::none;
};

/**
 *
 */
class Policies{
	public:
		rule get_policy(User::user_id, User::Group const&) const noexcept;
		rule get_policy(User::user_id, User::Groups const&) const noexcept;

		bool add(policy_id, User::group_id, rule) noexcept;

		void remove_group(User::group_id) noexcept;
	private:
		std::vector<Policy> policies_;
};

bool can(User::Logged const&, rule) noexcept;
bool can(User::User const&, rule) noexcept;

}//Authorization
}//Agro

#endif /* AGRO_DAEMON_USER_AUTH_POLICY_HPP__ */
