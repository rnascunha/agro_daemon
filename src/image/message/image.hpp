#ifndef AGRO_DAEMON_MESSAGE_OTA_HPP__
#define AGRO_DAEMON_MESSAGE_OTA_HPP__

#include <filesystem>

#include "rapidjson/document.h"

#include "../../message/types.hpp"
#include "../../instance/agro.hpp"

namespace Agro{
namespace Message {

enum class image_commands{
	erase = 0,
	list,
	edit
};

constexpr const ::Message::config<image_commands> img_config[] = {
	{image_commands::erase, "delete"},
	{image_commands::list, "list"},
	{image_commands::edit, "edit"}
};

inline constexpr ::Message::config<image_commands> const* get_config(image_commands t) noexcept
{
	for(std::size_t i = 0; i < sizeof(img_config) / sizeof(img_config[0]); i++)
	{
		if(t == img_config[i].mtype) return &img_config[i];
	}
	return nullptr;
}

inline constexpr ::Message::config<image_commands> const* get_image_config(const char* t) noexcept
{
	for(std::size_t i = 0; i < sizeof(img_config) / sizeof(img_config[0]); i++)
	{
		if(std::strcmp(t, img_config[i].name) == 0) return &img_config[i];
	}
	return nullptr;
}

void image_list(rapidjson::Document&, std::filesystem::path const&, instance&) noexcept;
std::string image_list(std::filesystem::path const&, instance&) noexcept;

}//Message
}//Agro

#endif /* AGRO_DAEMON_MESSAGE_OTA_HPP__ */
