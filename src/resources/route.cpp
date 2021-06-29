#include "../coap_engine.hpp"
#include "types.hpp"
#include <cstdio>
#include "../message/resource_requests.hpp"
#include "../message/device.hpp"

#include "../device/list.hpp"
#include "../websocket.hpp"

namespace Resource{

void put_route_handler(engine::message const& request,
								engine::response& response, void*,
								Device_List& device_list) noexcept
{
	std::printf("Put Route Handler\n");

	CoAP::Message::Option::option op;
	CoAP::Message::Option::get_option(request, op, CoAP::Message::Option::code::uri_host);
	CoAP::Message::Option::node uri_host{op};

	if(request.payload_len < sizeof(route))
	{
		response
			.code(CoAP::Message::code::bad_request)
			.add_option(uri_host)
			.payload("value too small")
			.serialize();
		return;
	}

	route const* rt = static_cast<route const*>(request.payload);
	std::size_t children_size = request.payload_len - sizeof(route);
	const std::uint8_t* children = static_cast<const uint8_t*>(request.payload) + sizeof(route);

	std::string str{Message::route_to_json(response.endpoint(), request, uri_host.value,
											*rt, children, children_size)};
	std::printf("JSON: %s\n", str.c_str());

	Error ec;
	::mesh_addr_t host{static_cast<const char*>(op.value), op.length, ec};
	auto& dev = device_list.update(host, response.endpoint(), *rt, children, children_size);
	std::printf("DEVICE: %s\n", Message::device_to_json(dev).c_str());

	Websocket<false>::write_all(str);

	if(request.mtype == CoAP::Message::type::confirmable)
	{
		CoAP::Message::Option::node uri_host{op};
		response
			.code(CoAP::Message::code::success)
			.add_option(uri_host)
			.serialize();
	}
}

}//Resource
