#include "types.hpp"
#include <iostream>
//#include "../../websocket/websocket.hpp"
#include "../../resources/process.hpp"

namespace Message{

static void packet_response(
		engine::endpoint const& ep,
		mesh_addr_t const& host,
		requests req,
		CoAP::Message::message const&,
		CoAP::Message::message const& response,
		CoAP::Transmission::status_t,
		Device_List& device_list) noexcept
{
	CoAP::Message::Option::option op;
	CoAP::Message::Option::get_option(response, op, CoAP::Message::Option::code::uri_host);

	switch(req)
	{
		case requests::sensor: {
			std::error_code ec;
			Resource::process_sensor_data(device_list,
					ep, op,
					response.payload, response.payload_len,
					ec);
		}
			break;
		case requests::board: {
			std::error_code ec;
			Resource::process_board(device_list,
					ep, op,
					response.payload, response.payload_len,
					ec);
		}
		break;
		case requests::config: {
			std::error_code ec;
			Resource::process_config(device_list,
					ep, op,
					response.payload, response.payload_len,
					ec);
		}
		break;
		case requests::full_config: {
			std::error_code ec;
			Resource::process_full_config(device_list,
					ep, op,
					response.payload, response.payload_len,
					ec);
		}
		break;
		case requests::route: {
			std::error_code ec;
			Resource::process_route(device_list,
					ep, op,
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
		{CoAP::Message::Option::code::uri_path, #sub}, 	\
		{CoAP::Message::Option::code::uri_path, #name}			\
	} 															\
}

REQUIRE_PACKET(sensor,packet);
REQUIRE_PACKET(board,packet);
REQUIRE_PACKET(route,net);
REQUIRE_PACKET(config,net);
REQUIRE_PACKET(full_config,net);

extern constexpr const request_config packet_sensor = {
	requests::sensor,
	"sensor",
	&req_sensor,
	packet_response
};
extern constexpr const request_config packet_board = {
	requests::board,
	"board",
	&req_board,
	packet_response
};
extern constexpr const request_config packet_route = {
	requests::route,
	"route",
	&req_route,
	packet_response
};
extern constexpr const request_config packet_config = {
	requests::config,
	"config",
	&req_config,
	packet_response
};
extern constexpr const request_config packet_full_config = {
	requests::full_config,
	"full_config",
	&req_full_config,
	packet_response
};

}//Message
