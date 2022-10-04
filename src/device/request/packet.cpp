#include "types.hpp"
#include "../../websocket/types.hpp"
#include "../resources/process.hpp"

namespace Agro{
namespace Device{
namespace Request{

static void packet_response(
		engine::endpoint const& ep,
		mesh_addr_t const& host,
		request_type req,
		CoAP::Message::message const&,
		CoAP::Message::message const& response,
		CoAP::Transmission::status_t,
		Agro::instance& instance,
		Agro::websocket_ptr) noexcept
{
	Agro::Device::Device* dev = instance.device_list()[host];
	switch(req)
	{
		break;
		case request_type::sensors: {
			std::error_code ec;
			Resource::process_sensors_data(*dev,
					instance,
					ep,
					response.payload, response.payload_len,
					ec);
		}
		break;
		case request_type::board: {
			std::error_code ec;
			Resource::process_board(*dev,
					instance,
					ep,
					response.payload, response.payload_len,
					ec);
		}
		break;
		case request_type::config: {
			std::error_code ec;
			Resource::process_config(*dev,
					instance,
					ep,
					response.payload, response.payload_len,
					ec);
		}
		break;
		case request_type::full_config: {
			std::error_code ec;
			Resource::process_full_config(*dev,
					instance,
					ep,
					response.payload, response.payload_len,
					ec);
		}
		break;
		case request_type::route: {
			std::error_code ec;
			Resource::process_route(*dev,
					instance,
					ep,
					response.payload, response.payload_len,
					ec);
		}
		break;
		default:
		break;
	}
}

#define REQUIRE_PACKET(name, sub) request_message const req_##name = { \
	CoAP::Message::code::get, 									\
	{ 															\
		{CoAP::Message::Option::code::uri_path, #sub}, 			\
		{CoAP::Message::Option::code::uri_path, #name}			\
	} 															\
}

REQUIRE_PACKET(sensors,packet);
REQUIRE_PACKET(board,packet);
REQUIRE_PACKET(route,net);
REQUIRE_PACKET(config,net);
REQUIRE_PACKET(full_config,net);

const request_config packet_sensors = {
	{request_type::sensors, "sensors"},
	&req_sensors,
	packet_response
};
const request_config packet_board = {
	{request_type::board, "board"},
	&req_board,
	packet_response
};
const request_config packet_route = {
	{request_type::route, "route"},
	&req_route,
	packet_response
};
const request_config packet_config = {
	{request_type::config, "config"},
	&req_config,
	packet_response
};
const request_config packet_full_config = {
	{request_type::full_config, "full_config"},
	&req_full_config,
	packet_response
};

}//Request
}//Device
}//Agro
