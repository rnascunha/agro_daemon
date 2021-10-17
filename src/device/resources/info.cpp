#include "../../coap_engine.hpp"
#include "../list.hpp"
#include "process.hpp"
#include "../../message/info.hpp"

namespace Agro{
namespace Device{
namespace Resource{

void put_info_handler(engine::message const& request,
					engine::response& response, void*,
					Agro::instance& instance) noexcept
{
	CoAP::Message::Option::option op;
	CoAP::Message::Option::get_option(request, op, CoAP::Message::Option::code::uri_host);

	instance.share()->write_all(std::make_shared<std::string>(
		::Message::make_info(::Message::info::info,
				op,
				std::string{static_cast<const char*>(request.payload), request.payload_len}.c_str()
		))
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
}//Device
}//Agro