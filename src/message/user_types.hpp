#ifndef AGRO_DAEMON_MESSAGE_USER_TYPES_HPP__
#define AGRO_DAEMON_MESSAGE_USER_TYPES_HPP__

#include <cstdlib>
#include <cstring>
//#include "types.hpp"
#include "../helper/utility.hpp"

namespace Message{

enum class user_commands{
	autheticate,
	auth_session_id,
	logout,
	push_subscribe,
	push_unsubscribe,
	user_group_policies,
	add_user,
	edit_user,
	delete_user,
	add_group,
	delete_group
};

constexpr const config<user_commands> user_config[] = {
	{user_commands::autheticate, "authenticate"},
	{user_commands::auth_session_id, "auth_session_id"},
	{user_commands::logout, "logout"},
	{user_commands::push_subscribe, "push_subscribe"},
	{user_commands::push_unsubscribe, "push_unsubscribe"},
	{user_commands::user_group_policies, "user_group_policies"},
	{user_commands::add_user, "add_user"},
	{user_commands::edit_user, "edit_user"},
	{user_commands::delete_user, "delete_user"},
	{user_commands::add_group, "add_group"},
	{user_commands::delete_group, "delete_group"}
};

inline constexpr auto get_user_config(const char* name)
{
    return ::get_config(name, user_config);
}

inline constexpr auto get_config(user_commands mtype)
{
    return ::get_config(mtype, user_config);
}

}//Message

#endif /* AGRO_DAEMON_MESSAGE_USER_TYPES_HPP__ */
