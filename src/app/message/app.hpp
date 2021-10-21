#ifndef AGRO_DAEMON_MESSAGE_APP_HPP__
#define AGRO_DAEMON_MESSAGE_APP_HPP__

#include "rapidjson/document.h"
#include <filesystem>
#include "../../message/types.hpp"
#include "../../instance/agro.hpp"

namespace Agro{
namespace Message {

enum class app_commands{
	erase = 0,
	list,
	edit
};

constexpr const ::Message::config<app_commands> app_config[] = {
	{app_commands::erase, "delete"},
	{app_commands::list, "list"},
	{app_commands::edit, "edit"}
};

inline constexpr ::Message::config<app_commands> const* get_config(app_commands t) noexcept
{
	for(std::size_t i = 0; i < sizeof(app_config) / sizeof(app_config[0]); i++)
	{
		if(t == app_config[i].mtype) return &app_config[i];
	}
	return nullptr;
}

inline constexpr ::Message::config<app_commands> const* get_app_config(const char* t) noexcept
{
	for(std::size_t i = 0; i < sizeof(app_config) / sizeof(app_config[0]); i++)
	{
		if(std::strcmp(t, app_config[i].name) == 0) return &app_config[i];
	}
	return nullptr;
}

void app_list(rapidjson::Document& doc, std::filesystem::path const& path, instance&) noexcept;
std::string app_list(std::filesystem::path const& path, instance&) noexcept;

}//Message
}//Agro

#endif /* AGRO_DAEMON_MESSAGE_APP_HPP__ */
