#include "types.hpp"
#include "../../websocket/types.hpp"
#include "../message/device.hpp"
#include "../message/esp_reset_reason.hpp"

namespace Agro{
namespace Device{
namespace Request{

static void process_uptime(Agro::instance& instance,
		mesh_addr_t const& host,
		int64_t uptime,
		Agro::websocket_ptr ws) noexcept
{
	auto* dev = instance.device_list()[host];
	if(!dev)
	{
		tt::error("[uptime] response error! Device '%s' not found", host.to_string().c_str());
		ws->write(instance.make_report(Agro::Message::report_type::error,
			host, "Device not found", "uptime", ws->user().id()));
		return;
	}
	dev->update_uptime(uptime);
	ws->write_all_policy(Agro::Authorization::rule::view_device,
			std::make_shared<std::string>(Agro::Device::Message::device_uptime_to_json(*dev)));
}

static void update_uptime_response(
		engine::endpoint const&,
		mesh_addr_t const& host,
		request_type,
		CoAP::Message::message const&,
		CoAP::Message::message const& response,
		CoAP::Transmission::status_t,
		Agro::instance& instance,
		Agro::websocket_ptr ws) noexcept
{
	std::string time{static_cast<const char*>(response.payload), response.payload_len};
	process_uptime(instance, host, std::strtoll(time.c_str(), nullptr, 10), ws);
}

static request_message const req_uptime = {
	CoAP::Message::code::get,
	{
		{CoAP::Message::Option::code::uri_path, "uptime"},
	}
};

static request_message const req_reset = {
	CoAP::Message::code::post,
	{
		{CoAP::Message::Option::code::uri_path, "reboot"},
	},
	CoAP::Message::type::nonconfirmable
};

extern constexpr const request_config uptime = {
	{request_type::uptime, "uptime"},
	&req_uptime,
	update_uptime_response
};
extern constexpr const request_config reset = {
	{request_type::reset, "reset"},
	&req_reset
};

static void reset_reason_response(
		engine::endpoint const&,
		mesh_addr_t const& host,
		request_type,
		CoAP::Message::message const&,
		CoAP::Message::message const& response,
		CoAP::Transmission::status_t,
		Agro::instance& instance,
		Agro::websocket_ptr ws) noexcept
{
	std::string reason{static_cast<const char*>(response.payload), response.payload_len};
	auto* dev = instance.device_list()[host];
	if(!dev)
	{
		tt::error("[reset_reason] response error! Device '%s' not found", host.to_string().c_str());
		ws->write(instance.make_report(Agro::Message::report_type::error,
			host, "Device not found", "reset_reason", ws->user().id()));
		return;
	}

	long reason_num = std::strtoll(reason.c_str(), nullptr, 10);
	tt::status("[%s] reset_reason: %d / %s [%s]",
			host.to_string().c_str(),
			reason_num,
			reset_reason_string(static_cast<esp_reset_reason_t>(reason_num)),
			reset_reason_string_short(static_cast<esp_reset_reason_t>(reason_num)));
	ws->write_all_policy(Agro::Authorization::rule::view_device,
		std::make_shared<std::string>(Agro::Device::Message::device_reset_reason_to_json(*dev, reason_num)));
}

static request_message const req_reset_reason = {
	CoAP::Message::code::get,
	{
		{CoAP::Message::Option::code::uri_path, "reset_reason"}
	}
};

extern constexpr const request_config reset_reason = {
	{request_type::reset_reason, "reset_reason"},
	&req_reset_reason,
	reset_reason_response
};

}//Request
}//Device
}//Agro
