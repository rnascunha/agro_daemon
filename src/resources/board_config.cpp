#include "../coap_engine.hpp"
#include "types.hpp"
#include <cstdio>
#include "../message/resource_requests.hpp"

#include "../error.hpp"

#include "../websocket.hpp"

static void put_board_config_handler(engine::message const& request,
								engine::response& response, void*) noexcept
{
	std::printf("Put Board Config Handler\n");

	CoAP::Message::Option::option op;
	CoAP::Message::Option::get_option(request, op, CoAP::Message::Option::code::uri_host);
	CoAP::Message::Option::node uri_host{op};

	if(request.payload_len < sizeof(board_config))
	{
		response
			.code(CoAP::Message::code::bad_request)
			.add_option(uri_host)
			.payload("value too small")
			.serialize();
		return;
	}

	board_config const* rt = static_cast<board_config const*>(request.payload);
	std::string str{Message::board_config_to_json(response.endpoint(),
										request,
										uri_host.value,
										*rt,
										static_cast<const char*>(request.payload) + sizeof(board_config),
										request.payload_len - sizeof(board_config))};
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

engine::resource_node res_board_config{"board",
								"Get board configuration",
								nullptr,
								nullptr,
								put_board_config_handler};
