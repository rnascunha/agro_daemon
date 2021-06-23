#include "../coap_engine.hpp"
#include "types.hpp"
#include <cstdio>
#include "data_to_json.hpp"

#include "../websocket.hpp"

static void put_route_handler(engine::message const& request,
								engine::response& response, void*) noexcept
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

	std::string str{route_to_json(response.endpoint(), uri_host, *rt, children, children_size)};
	std::printf("JSON: %s", str.c_str());

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

engine::resource_node res_route{"route",
								"Get devices route",
								nullptr,
								nullptr,
								put_route_handler};
