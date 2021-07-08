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

/**
 * Requests
 */
enum class requests{
	custom,      /**< custom */      /**< custom */
	ac_load1_on, /**< ac_load1_on */ /**< ac_load1_on */
	ac_load1_off,/**< ac_load1_off *//**< ac_load1_off */
	ac_load2_on, /**< ac_load2_on */ /**< ac_load2_on */
	ac_load2_off,/**< ac_load2_off *//**< ac_load2_off */
	ac_load3_on, /**< ac_load3_on */ /**< ac_load3_on */
	ac_load3_off,/**< ac_load3_off *//**< ac_load3_off */
	reset,       /**< reset */       /**< reset */
	uptime,      /**< uptime */      /**< uptime */
	get_rtc,     /**< get_rtc */     /**< get_rtc */
	update_rtc,  /**< update_rtc */  /**< update_rtc */
	sensor,      /**< sensor */      /**< sensor */
	route,       /**< route */       /**< route */
	board,       /**< board */       /**< board */
	config,      /**< config */      /**< config */
	full_config, /**< full_config */ /**< full_config */
	get_ota,     /**< get_ota */     /**< get_ota */
	update_ota   /**< update_ota */  /**< update_ota */
};

struct request_message
{
	CoAP::Message::code		method;
	std::vector<CoAP::Message::Option::option> options;
	CoAP::Message::type		mtype = CoAP::Message::type::confirmable;
	std::function<std::size_t(rapidjson::Document const&, void*, std::size_t)> payload = nullptr;
};

struct request_config{
	requests	mtype;
	const char* name;
	request_message const* message;
};

#define AC_LOAD(name,num,value) request_message const req_##name = { \
	CoAP::Message::code::put, 									\
	{ 															\
		{CoAP::Message::Option::code::uri_path, "ac_load"}, 	\
		{CoAP::Message::Option::code::uri_path, #num}, 			\
		{CoAP::Message::Option::code::uri_query, "value=" value}, 	\
	} 															\
}

AC_LOAD(ac1_on, 1, "1");
AC_LOAD(ac1_off, 1, "0");
AC_LOAD(ac2_on, 2, "1");
AC_LOAD(ac2_off, 2, "0");
AC_LOAD(ac3_on, 1, "1");
AC_LOAD(ac3_off, 1, "0");

request_message const req_reset = {
	CoAP::Message::code::post,
	{
		{CoAP::Message::Option::code::uri_path, "reboot"},
	},
	CoAP::Message::type::nonconfirmable
};

request_message const req_uptime = {
	CoAP::Message::code::get,
	{
		{CoAP::Message::Option::code::uri_path, "uptime"},
	}
};

static CoAP::Message::content_format rtc_update_content = CoAP::Message::content_format::application_octet_stream;
static CoAP::Message::content_format rtc_get_content = CoAP::Message::content_format::application_octet_stream;

request_message const req_update_rtc = {
	CoAP::Message::code::put,
	{
		{rtc_update_content},
		{CoAP::Message::Option::code::uri_path, "rtc"}
	},
	CoAP::Message::type::confirmable,
	[](rapidjson::Document const&, void* buf, std::size_t size) -> auto
	{
		std::uint32_t time = get_time();
		std::memcpy(buf, &time, sizeof(std::uint32_t));

		return sizeof(std::uint32_t);
	}
};

request_message const req_get_rtc = {
	CoAP::Message::code::get,
	{
		{rtc_get_content, true},
		{CoAP::Message::Option::code::uri_path, "rtc"},
	}
};

#define REQUIRE_PACKET(name, sub) request_message const req_##name = { \
	CoAP::Message::code::get, 									\
	{ 															\
		{CoAP::Message::Option::code::uri_path, #sub}, 	\
		{CoAP::Message::Option::code::uri_path, #name}			\
	} 															\
}

REQUIRE_PACKET(sensor,packet);
REQUIRE_PACKET(board,packet);
REQUIRE_PACKET(route,net);
REQUIRE_PACKET(config,net);
REQUIRE_PACKET(full_config,net);

request_message const req_get_ota = {
	CoAP::Message::code::get,
	{
		{CoAP::Message::Option::code::uri_path, "ota"},
	}
};

request_message const req_update_ota = {
	CoAP::Message::code::post,
	{
		{CoAP::Message::Option::code::uri_path, "ota"},
	},
	CoAP::Message::type::confirmable,
	[](rapidjson::Document const& doc, void* buf, std::size_t size) -> auto
	{
		std::size_t s = 0;
		if(doc.HasMember("payload") && doc["payload"].IsString())
		{
			const char* c = doc["payload"].GetString();
			s = std::strlen(c);
			std::memcpy(buf, c, s);
		}
		return s;
	}
};

constexpr const request_config rconfig[] = {
	{requests::custom, "custom", {}},
	{requests::ac_load1_on, "ac1_on", &req_ac1_on},
	{requests::ac_load1_off, "ac1_off", &req_ac1_off},
	{requests::ac_load2_on, "ac2_on", &req_ac2_on},
	{requests::ac_load2_off, "ac2_off", &req_ac2_off},
	{requests::ac_load3_on, "ac3_on", &req_ac3_on},
	{requests::ac_load3_off, "ac3_off", &req_ac3_off},
	{requests::reset, "reset", &req_reset},
	{requests::uptime, "uptime", &req_uptime},
	{requests::get_rtc, "get_rtc", &req_get_rtc},
	{requests::update_rtc, "update_rtc", &req_update_rtc},
	{requests::sensor, "sensor", &req_sensor},
	{requests::route, "route", &req_route},
	{requests::board, "board", &req_board},
	{requests::config, "config", &req_config},
	{requests::full_config, "full_config", &req_full_config},
	{requests::get_ota, "get_ota_version", &req_get_ota},
	{requests::update_ota, "update_ota", &req_update_ota},
};

inline constexpr request_config const* get_requests_config(requests t) noexcept
{
	for(std::size_t i = 0; i < sizeof(rconfig) / sizeof(rconfig[0]); i++)
	{
		if(t == rconfig[i].mtype) return &rconfig[i];
	}
	return nullptr;
}

inline constexpr request_config const* get_requests_config(const char* t) noexcept
{
	for(std::size_t i = 0; i < sizeof(rconfig) / sizeof(rconfig[0]); i++)
	{
		if(std::strcmp(t, rconfig[i].name) == 0) return &rconfig[i];
	}
	return nullptr;
}

/**
 * Info
 */
enum class info{
	error = 0,
	warning,
	success,
	info
};

constexpr const config<info> info_config[] = {
	{info::error, "error"},
	{info::warning, "warning"},
	{info::success, "success"},
	{info::info, "info"},
};

inline constexpr config<info> const* get_config(info t) noexcept
{
	for(std::size_t i = 0; i < sizeof(info_config) / sizeof(info_config[0]); i++)
	{
		if(t == info_config[i].mtype) return &info_config[i];
	}
	return nullptr;
}

//inline constexpr config<info> const* get_config(const char* t) noexcept
//{
//	for(std::size_t i = 0; i < sizeof(info_config) / sizeof(info_config[0]); i++)
//	{
//		if(std::strcmp(t, info_config[i].name) == 0) return &info_config[i];
//	}
//	return nullptr;
//}

}//Message

#endif /* AGRO_DAEMON_MESSAGE_TYPES_HPP__ */