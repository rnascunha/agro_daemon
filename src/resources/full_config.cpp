#include "../coap_engine.hpp"
#include "types.hpp"
#include <cstdio>
#include "../message/resource_requests.hpp"

#include "../websocket.hpp"
#include "../device/list.hpp"

namespace Resource{

void put_full_config_handler(engine::message const& request,
								engine::response& response, void*,
								Device_List& device_list) noexcept
{
	std::printf("Put Full Config Handler\n");

	CoAP::Message::Option::option op;
	CoAP::Message::Option::get_option(request, op, CoAP::Message::Option::code::uri_host);
	CoAP::Message::Option::node uri_host{op};

	if(request.payload_len < sizeof(full_config))
	{
		response
			.code(CoAP::Message::code::bad_request)
			.add_option(uri_host)
			.payload("value too small")
			.serialize();
		return;
	}

	full_config const* rt = static_cast<full_config const*>(request.payload);
	std::size_t children_size = request.payload_len - sizeof(route) - sizeof(status) - sizeof(config);
	const std::uint8_t* children = static_cast<const uint8_t*>(request.payload)
			+ sizeof(route) + sizeof(status) + sizeof(config);

	std::string str{Message::full_config_to_json(response.endpoint(), request,
					uri_host.value, *rt, children, children_size)};
	std::printf("JSON: %s", str.c_str());

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
