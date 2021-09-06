#ifndef AGRO_DAEMON_MESSAGE_USER_TYPES_HPP__
#define AGRO_DAEMON_MESSAGE_USER_TYPES_HPP__

#include <cstdlib>
#include <cstring>
#include "types.hpp"

namespace Message{

enum class user_commands{
	autheticate,
	auth_session_id,
	logout,
	push_subscribe,
	push_unsubscribe
};

constexpr const config<user_commands> user_config[] = {
	{user_commands::autheticate, "authenticate"},
	{user_commands::auth_session_id, "auth_session_id"},
	{user_commands::logout, "logout"},
	{user_commands::push_subscribe, "push_subscribe"},
	{user_commands::push_unsubscribe, "push_unsubscribe"},
};

inline constexpr config<user_commands> const* get_config(user_commands t) noexcept
{
	for(std::size_t i = 0; i < sizeof(user_config) / sizeof(user_config[0]); i++)
	{
		if(t == user_config[i].mtype) return &user_config[i];
	}
	return nullptr;
}

inline constexpr config<user_commands> const* get_user_config(const char* t) noexcept
{
	for(std::size_t i = 0; i < sizeof(user_config) / sizeof(user_config[0]); i++)
	{
		if(std::strcmp(t, user_config[i].name) == 0) return &user_config[i];
	}
	return nullptr;
}

}//Message

#endif /* AGRO_DAEMON_MESSAGE_USER_TYPES_HPP__ */
