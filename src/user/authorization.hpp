#ifndef AGRO_DAEMON_USER_AUTHORIZATION_HPP__
#define AGRO_DAEMON_USER_AUTHORIZATION_HPP__

#include <limits>
#include <vector>

#include "user.hpp"
#include "group.hpp"

#include <unordered_map>

#include "../helper/enum_ops.hpp"

namespace Agro{

enum class auth_type : int{
	no_auth,
	//User
	create_user = 1 << 0,
	view_user = 1 << 1,
	reset_user_password = 1 << 2,
	delete_user = 1 << 3,
	//create_user_group,
	//delete_user_group,
	//add_user_to_group,
	//remove_user_from_group,
	//become_root,
	all_user = create_user
				| view_user
				| reset_user_password
				| delete_user,
	//Device
	create_device = 1 << 4,
	view_device = 1 << 5,
	update_device = 1 << 6, 	//Update image
	install_app = 1 << 7,
	send_command = 1 << 8,
	delete_device = 1 << 9,
	all_device = create_device
				| view_device
				| update_device
				| install_app
				| send_command
				| delete_device,
	//Net
	view_net = 1 << 10,
	update_net_device = 1 << 11,
	install_app_net_device = 1 << 12,
	send_command_net_device = 1 << 13,
	delete_net_device = 1 << 14,
	all_net_device = view_net
				| update_net_device
				| install_app_net_device
				| send_command_net_device
				| delete_net_device,
	//Group Device
	//create_device_group
	//delete_device_group
	//add_device_to_group
	//remove_device_from_group
	view_group = 1 << 15,
	update_group_device = 1 << 16,
	install_app_group_device = 1 << 17,
	send_command_group_device = 1 << 18,
	delete_group_device = 1 << 19,
	all_group_device = view_group
				| update_group_device
				| install_app_group_device
				| send_command_group_device
				| delete_group_device,
	//Image
	upload_image = 1 << 20,
	delete_image = 1 << 21,
	all_image = upload_image
				| delete_image,
	//App
	upload_app = 1 << 22,
	delete_app = 1 << 23,
	all_app = upload_app
			 	 | delete_app,
	all = std::numeric_limits<int>()
};

static constexpr const int permission_any = -1;

/**
 * Contains only one permission
 */
class Permission{
	public:
		int			id() const noexcept;
		auth_type	type() const noexcept;
		int			reference_id() const noexcept;

		bool can(auth_type type, int ref_id) const noexcept
		{
			if(type_ == type
				&& (ref_id_ == ref_id
					|| ref_id == permission_any))
			{
				return true;
			}
			return false;
		}
	private:
		int 				id_;
		auth_type			type_;
		int					ref_id_ = permission_any;
};

/**
 * Contains a group of permission
 */
class Permission_Group{
	public:
		bool can(auth_type type, int ref_id) const noexcept
		{
			for(auto const& p : perm_)
			{
				if(p.can(type, ref_id))
				{
					return true;
				}
			}
			return false;
		}
	private:
		int							id_;
		std::vector<Permission&> 	perm_;
};

/**
 * Contains all permissions
 */
class Permission_List{
	public:
	private:
		std::vector<Permission> 		perm_;
		std::vector<Permission_Group> 	gperm_;
};

class User_Permission{
	public:
		bool can(auth_type type, int ref_id) const noexcept
		{
			for(auto const& p : perm_)
			{
				if(can(type, ref_id))
				{
					return true;
				}
			}

			for(auto const& p : gperm_)
			{
				if(p.can(type, ref_id))
				{
					return true;
				}
			}
			return false;
		}
	private:
		int								id_;
		std::vector<Permission&> 		perm_;
		std::vector<Permission_Group&> 	gperm_;
};

class Auth{
	public:
		bool can(int user_id, auth_type type, int reference_id) const noexcept
		{
			auto user_perm = user_perm_.find(user_id);
			if(user_perm != user_perm_.end())
			{
				if(user_perm->second.can(type, reference_id))
				{
					return true;
				}
			}
			for(auto const& group : user_groups_)
			{
				if(group.contains(user_id))
				{
					auto group_perm = group_perm_.find(group.id());
					if(group_perm != group_perm_.end())
					{
						if(group_perm->second.can(type, reference_id))
						{
							return true;
						}
					}
				}
			}
			return false;
		}
	private:
		Permission_List	list_;
		std::unordered_map<int /*userid*/,
							User_Permission>	user_perm_;
		std::unordered_map<int /*groupid*/,
							User_Permission>	group_perm_;
		User_Group_List&						user_groups_;
};

}//Agro

#endif /* AGRO_DAEMON_USER_AUTHORIZATION_HPP__ */
