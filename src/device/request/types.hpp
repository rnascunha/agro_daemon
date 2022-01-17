#ifndef AGRO_DAEMON_DEVICE_REQUEST_TYPES_HPP__
#define AGRO_DAEMON_DEVICE_REQUEST_TYPES_HPP__

#include <vector>

#include "request_list.hpp"
#include "../../coap_engine.hpp"
#include "../../instance/agro.hpp"
#include "../../websocket/types.hpp"

#include "../../helper/utility.hpp"

namespace Agro{
namespace Device{
namespace Request{

struct request_message
{
	CoAP::Message::code		method;
	std::vector<CoAP::Message::Option::option> options;
	CoAP::Message::type		mtype = CoAP::Message::type::confirmable;
	std::function<std::size_t(rapidjson::Document const&,
							void*, std::size_t, instance&,
							std::error_code&)> payload = nullptr;
};

struct request_config{
	type	mtype;
	const char* name;
	request_message const* message;
	void(*response)(engine::endpoint const&,
			mesh_addr_t const&,
			type,
			CoAP::Message::message const&,
			CoAP::Message::message const&,
			CoAP::Transmission::status_t,
			instance&,
			websocket_ptr) noexcept = nullptr;
};

extern const request_config custom;
extern const request_config uptime;
extern const request_config reset;
extern const request_config reset_reason;
extern const request_config waive_root;
extern const request_config ac1_on;
extern const request_config ac1_off;
extern const request_config ac2_on;
extern const request_config ac2_off;
extern const request_config ac3_on;
extern const request_config ac3_off;
extern const request_config packet_sensors;
extern const request_config packet_board;
extern const request_config packet_config;
extern const request_config packet_route;
extern const request_config packet_full_config;
extern const request_config send_job;
extern const request_config get_job;
extern const request_config delete_job;
extern const request_config get_rtc;
extern const request_config update_rtc;
extern const request_config get_fuse;
extern const request_config update_fuse;
extern const request_config get_ota;
extern const request_config update_ota;
extern const request_config get_app;
extern const request_config send_app;
extern const request_config execute_app;
extern const request_config delete_app;

const request_config rconfig[] = {
	custom,
	uptime,
	reset,
	reset_reason,
	waive_root,
	ac1_on,
	ac1_off,
	ac2_on,
	ac2_off,
	ac3_on,
	ac3_off,
	packet_sensors,
	packet_board,
	packet_config,
	packet_route,
	packet_full_config,
	send_job,
	get_job,
	delete_job,
	get_rtc,
	update_rtc,
	get_fuse,
	update_fuse,
	get_ota,
	update_ota,
	get_app,
	send_app,
	execute_app,
	delete_app
};

inline constexpr request_config const* get_requests_config(type t) noexcept
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
//		if(std::strcmp(t, rconfig[i].name) == 0) return &rconfig[i];
		if(is_equal(t, rconfig[i].name)) return &rconfig[i];
	}
	return nullptr;
}

}//Request
}//Device
}//Agro

#endif /* AGRO_DAEMON_DEVICE_REQUEST_TYPES_HPP__ */
