#ifndef AGRO_DAEMON_MESSAGE_APP_HPP__
#define AGRO_DAEMON_MESSAGE_APP_HPP__

#include "rapidjson/document.h"
#include <filesystem>
//#include "../../message/types.hpp"
#include "../../instance/agro.hpp"

#include "../../helper/utility.hpp"

namespace Agro{
namespace Message {

enum class app_commands{
	erase = 0,
	list,
	edit,
	download,
};

constexpr const config<app_commands> app_config[] = {
	{app_commands::erase, "delete"},
	{app_commands::list, "list"},
	{app_commands::edit, "edit"},
	{app_commands::download, "download"}
};

inline constexpr auto get_app_config(const char* name) noexcept
{
    return ::get_config(name, app_config);
}

void app_list(rapidjson::Document& doc, std::filesystem::path const& path, instance&) noexcept;
std::string app_list(std::filesystem::path const& path, instance&) noexcept;

}//Message
}//Agro

#endif /* AGRO_DAEMON_MESSAGE_APP_HPP__ */
