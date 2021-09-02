#ifndef AGRO_DAEMON_MESSAGE_USER_HPP__
#define AGRO_DAEMON_MESSAGE_USER_HPP__

#include "../user/user.hpp"
#include "types.hpp"
#include "rapidjson/document.h"

namespace Message{

enum class user_commands{
	autheticate,
	auth_session_id,
	logout,
	subscription
};

constexpr const config<user_commands> user_config[] = {
	{user_commands::autheticate, "authenticate"},
	{user_commands::auth_session_id, "auth_session_id"},
	{user_commands::logout, "logout"},
	{user_commands::subscription, "subscription"},
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

void process_user(rapidjson::Document const&) noexcept;
std::string user_error_authentication(std::error_code const&) noexcept;
std::string user_authentication(Agro::User const&) noexcept;

}//Message

#endif /* AGRO_DAEMON_MESSAGE_USER_HPP__ */
