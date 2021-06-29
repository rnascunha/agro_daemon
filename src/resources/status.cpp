#include "../coap_engine.hpp"
#include "types.hpp"
#include <cstdio>
#include "../message/resource_requests.hpp"
#include "../message/device.hpp"

#include "../websocket.hpp"
#include "../device/list.hpp"

namespace Resource{

void put_status_handler(engine::message const& request,
								engine::response& response, void*,
								Device_List& device_list) noexcept
{
	std::printf("Put Status Handler\n");

	CoAP::Message::Option::option op;
	CoAP::Message::Option::get_option(request, op, CoAP::Message::Option::code::uri_host);
	CoAP::Message::Option::node uri_host{op};

	if(request.payload_len < sizeof(status))
	{
		response
			.code(CoAP::Message::code::bad_request)
			.add_option(uri_host)
			.payload("value too small")
			.serialize();
		return;
	}

	status const* rt = static_cast<status const*>(request.payload);
	std::string str{Message::status_to_json(response.endpoint(), request,
											uri_host.value, *rt)};
	std::printf("JSON: %s\n", str.c_str());

	Error ec;
	::mesh_addr_t host{static_cast<const char*>(op.value), op.length, ec};
	auto& dev = device_list.update(host, response.endpoint(), *rt);
	std::printf("DEVICE: %s\n", Message::device_to_json(dev).c_str());

	Websocket<false>::write_all(str);

	if(request.mtype == CoAP::Message::type::confirmable)
	{
		response
			.code(CoAP::Message::code::success)
			.add_option(uri_host)
			.serialize();
	}
}

}//Resource
