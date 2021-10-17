#include "types.hpp"
#include "../../websocket/types.hpp"
#include "../../device/message/device.hpp"

namespace Agro{
namespace Device{
namespace Request{

static void process_uptime(Agro::Device::Device_List& device_list,
		mesh_addr_t const& host,
		int64_t uptime,
		Agro::websocket_ptr ws) noexcept
{
	auto* dev = device_list[host];
	ws->write_all_policy(Agro::Authorization::rule::view_device,
			std::make_shared<std::string>(Agro::Device::Message::device_uptime_to_json(*dev)));
}

static void update_ota_response(
		engine::endpoint const&,
		mesh_addr_t const& host,
		type,
		CoAP::Message::message const&,
		CoAP::Message::message const& response,
		CoAP::Transmission::status_t,
		Agro::instance& instance,
		Agro::websocket_ptr ws) noexcept
{
	std::string time{static_cast<const char*>(response.payload), response.payload_len};
	process_uptime(instance.device_list(), host, std::strtoll(time.c_str(), nullptr, 10), ws);
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
	type::uptime,
	"uptime",
	&req_uptime,
	update_ota_response
};
extern constexpr const request_config reset = {
	type::reset,
	"reset",
	&req_reset
};

}//Request
}//Device
}//Agro
