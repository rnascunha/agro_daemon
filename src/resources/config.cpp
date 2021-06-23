#include "../coap_engine.hpp"
#include "types.hpp"
#include <cstdio>
#include "data_to_json.hpp"
#include "../error.hpp"

#include "../websocket.hpp"

static void put_config_handler(engine::message const& request,
								engine::response& response, void*) noexcept
{
	std::printf("Put Config Handler\n");

	CoAP::Message::Option::option op;
	CoAP::Message::Option::get_option(request, op, CoAP::Message::Option::code::uri_host);
	CoAP::Message::Option::node uri_host{op};

	if(request.payload_len < sizeof(config))
	{
		response
			.code(CoAP::Message::code::bad_request)
			.add_option(uri_host)
			.payload("value too small")
			.serialize();
		return;
	}

	config const* rt = static_cast<config const*>(request.payload);
	std::string str{config_to_json(response.endpoint(), uri_host, *rt)};
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

engine::resource_node res_config{"config",
								"Get devices configuration",
								nullptr,
								nullptr,
								put_config_handler};
