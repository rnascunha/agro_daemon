#include "../../coap_engine.hpp"
#include "../../device/list.hpp"
#include <vector>

namespace Message{

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
	get_rtc,
	update_rtc,
	get_fuse,
	update_fuse,
	sensor,
	route,
	board,
	config,
	full_config,
	get_ota,
	update_ota,
	send_job,
	get_job,
	delete_job,
};

struct request_message
{
	CoAP::Message::code		method;
	std::vector<CoAP::Message::Option::option> options;
	CoAP::Message::type		mtype = CoAP::Message::type::confirmable;
	std::function<std::size_t(rapidjson::Document const&, void*, std::size_t, std::error_code&)> payload = nullptr;
};

struct request_config{
	requests	mtype;
	const char* name;
	request_message const* message;
	void(*response)(engine::endpoint const&,
			mesh_addr_t const&,
			requests,
			CoAP::Message::message const&,
			CoAP::Message::message const&,
			CoAP::Transmission::status_t,
			Device_List&) noexcept = nullptr;
};

extern const request_config custom;
extern const request_config uptime;
extern const request_config reset;
extern const request_config ac1_on;
extern const request_config ac1_off;
extern const request_config ac2_on;
extern const request_config ac2_off;
extern const request_config ac3_on;
extern const request_config ac3_off;
extern const request_config packet_sensor;
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

const request_config rconfig[] = {
	custom,
	uptime,
	reset,
	ac1_on,
	ac1_off,
	ac2_on,
	ac2_off,
	ac3_on,
	ac3_off,
	packet_sensor,
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

}//Namespace