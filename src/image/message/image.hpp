#ifndef AGRO_DAEMON_MESSAGE_OTA_HPP__
#define AGRO_DAEMON_MESSAGE_OTA_HPP__

#include <filesystem>

#include "rapidjson/document.h"

//#include "../../message/types.hpp"
#include "../../instance/agro.hpp"

#include "../../helper/utility.hpp"

namespace Agro{
namespace Message {

enum class image_commands{
	erase = 0,
	list,
	edit,
	download,
};

constexpr const config<image_commands> img_config[] = {
	{image_commands::erase, "delete"},
	{image_commands::list, "list"},
	{image_commands::edit, "edit"},
	{image_commands::download, "download"}
};

inline constexpr auto get_image_config(const char* name) noexcept
{
    return ::get_config(name, img_config);
}

inline constexpr auto get_config(image_commands mtype) noexcept
{
    return ::get_config(mtype, img_config);
}

void image_list(rapidjson::Document&, std::filesystem::path const&, instance&) noexcept;
std::string image_list(std::filesystem::path const&, instance&) noexcept;

}//Message
}//Agro

#endif /* AGRO_DAEMON_MESSAGE_OTA_HPP__ */
