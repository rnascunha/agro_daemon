#include "../coap_engine.hpp"
#include "../device/list.hpp"
#include "process.hpp"
#include "../message/info.hpp"

#include "../websocket/websocket.hpp"

namespace Resource{

void put_info_handler(engine::message const& request,
								engine::response& response, void*) noexcept
{
	CoAP::Message::Option::option op;
	CoAP::Message::Option::get_option(request, op, CoAP::Message::Option::code::uri_host);

	Websocket<false>::write_all(
		Message::make_info(Message::info::info,
				op,
				std::string{static_cast<const char*>(request.payload), request.payload_len}.c_str()
		)
	);

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
