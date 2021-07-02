#ifndef AGRO_DAEMON_MESSAGE_TYPES_HPP__
#define AGRO_DAEMON_MESSAGE_TYPES_HPP__

#include <cstdlib>
#include <cstring>
#include "coap-te.hpp"
#include <vector>

namespace Message{

template<typename Type>
struct config{
	Type		mtype;
	const char* name;
};

enum class type{
	resource = 0,
	response,
	request,
	device,
	device_list,
	command,
	error
};

constexpr const config<type> tconfig[] = {
		{type::resource, "resource"},
		{type::response, "response"},
		{type::request, "request"},
		{type::device, "device"},
		{type::device_list, "device_list"},
		{type::command, "command"},
		{type::error, "error"}
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

enum class commands{
	update_name = 0
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

enum class requests{
	custom,
	ac_load1_on,
	ac_load1_off,
	ac_load2_on,
	ac_load2_off,
	ac_load3_on,
	ac_load3_off,
	reset,
	uptime,
	sensor,
	route,
	board,
	config,
	full_config
};

struct request_message
{
	CoAP::Message::code		method;
	std::vector<CoAP::Message::Option::option> options;
	CoAP::Message::type		mtype = CoAP::Message::type::confirmable;
	const std::uint8_t* 	payload = nullptr;
	std::size_t				payload_len = 0;
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
	{requests::sensor, "sensor", &req_sensor},
	{requests::route, "route", &req_route},
	{requests::board, "board", &req_board},
	{requests::config, "config", &req_config},
	{requests::full_config, "full_config", &req_full_config}
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

}//Message

#endif /* AGRO_DAEMON_MESSAGE_TYPES_HPP__ */
