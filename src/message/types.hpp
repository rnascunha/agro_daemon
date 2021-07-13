#ifndef AGRO_DAEMON_MESSAGE_TYPES_HPP__
#define AGRO_DAEMON_MESSAGE_TYPES_HPP__

#include <cstdlib>
#include <cstring>
#include <cstdint>
#include <functional>
#include <vector>

#include "rapidjson/document.h"
#include "../device/types.hpp"
#include "coap-te.hpp"
#include "../error.hpp"


namespace Message{

template<typename Type>
struct config{
	Type		mtype;
	const char* name;
};

/**
 * Types
 */
enum class type{
	resource = 0,/**< resource */
	response,    /**< response */
	request,     /**< request */
	device,      /**< device */
	device_list, /**< device_list */
	command,     /**< command */
	image,       /**< image */
	info        /**< info */
};

constexpr const config<type> tconfig[] = {
	{type::resource, "resource"},
	{type::response, "response"},
	{type::request, "request"},
	{type::device, "device"},
	{type::device_list, "device_list"},
	{type::command, "command"},
	{type::image, "image"},
	{type::info, "info"}
};


inline constexpr config<type> const* get_type_config(type t) noexcept
{
	for(std::size_t i = 0; i < sizeof(tconfig) / sizeof(tconfig[0]); i++)
	{
		if(t == tconfig[i].mtype) return &tconfig[i];
	}
	return nullptr;
}

inline constexpr config<type> const* get_type_config(const char* t) noexcept
{
	for(std::size_t i = 0; i < sizeof(tconfig) / sizeof(tconfig[0]); i++)
	{
		if(std::strcmp(t, tconfig[i].name) == 0) return &tconfig[i];
	}
	return nullptr;
}

/**
 * Commands
 */
enum class commands{
	update_name = 0/**< update_name */
};

constexpr const config<commands> cconfig[] = {
		{commands::update_name, "update_name"},
};

inline constexpr config<commands> const* get_commands_config(commands t) noexcept
{
	for(std::size_t i = 0; i < sizeof(cconfig) / sizeof(cconfig[0]); i++)
	{
		if(t == cconfig[i].mtype) return &cconfig[i];
	}
	return nullptr;
}

inline constexpr config<commands> const* get_commands_config(const char* t) noexcept
{
	for(std::size_t i = 0; i < sizeof(cconfig) / sizeof(cconfig[0]); i++)
	{
		if(std::strcmp(t, cconfig[i].name) == 0) return &cconfig[i];
	}
	return nullptr;
}

/**
 * Image
 */
enum class image_commands{
	erase = 0,
	list
};

constexpr const config<image_commands> img_config[] = {
	{image_commands::erase, "delete"},
	{image_commands::list, "list"},
};

inline constexpr config<image_commands> const* get_config(image_commands t) noexcept
{
	for(std::size_t i = 0; i < sizeof(img_config) / sizeof(img_config[0]); i++)
	{
		if(t == img_config[i].mtype) return &img_config[i];
	}
	return nullptr;
}

inline constexpr config<image_commands> const* get_config(const char* t) noexcept
{
	for(std::size_t i = 0; i < sizeof(img_config) / sizeof(img_config[0]); i++)
	{
		if(std::strcmp(t, img_config[i].name) == 0) return &img_config[i];
	}
	return nullptr;
}

}//Message

#endif /* AGRO_DAEMON_MESSAGE_TYPES_HPP__ */
