#ifndef AGRO_DAEMON_MESSAGE_TYPES_HPP__
#define AGRO_DAEMON_MESSAGE_TYPES_HPP__

#include <cstdlib>
#include <cstring>

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
	command,     /**< command */
	image,       /**< image */
	app,		/**< app */
	user,		/**< user */
	info        /**< info */
};

constexpr const config<type> tconfig[] = {
	{type::resource, "resource"},
	{type::response, "response"},
	{type::request, "request"},
	{type::device, "device"},
	{type::command, "command"},
	{type::image, "image"},
	{type::app, "app"},
	{type::user, "user"},
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

/**
 * App
 */
enum class app_commands{
	erase = 0,
	list
};

constexpr const config<app_commands> app_config[] = {
	{app_commands::erase, "delete"},
	{app_commands::list, "list"},
};

inline constexpr config<app_commands> const* get_config(app_commands t) noexcept
{
	for(std::size_t i = 0; i < sizeof(app_config) / sizeof(app_config[0]); i++)
	{
		if(t == app_config[i].mtype) return &app_config[i];
	}
	return nullptr;
}

inline constexpr config<app_commands> const* get_app_config(const char* t) noexcept
{
	for(std::size_t i = 0; i < sizeof(app_config) / sizeof(app_config[0]); i++)
	{
		if(std::strcmp(t, app_config[i].name) == 0) return &app_config[i];
	}
	return nullptr;
}

}//Message

#endif /* AGRO_DAEMON_MESSAGE_TYPES_HPP__ */
