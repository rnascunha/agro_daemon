#include "../coap_engine.hpp"
#include "types.hpp"
#include <cstdio>
#include "../message/resource_requests.hpp"

//#include "data_to_json.hpp"
#include "../websocket.hpp"

static void put_status_handler(engine::message const& request,
								engine::response& response, void*) noexcept
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

engine::resource_node res_status{"status",
								"Get devices status",
								nullptr,
								nullptr,
								put_status_handler};
