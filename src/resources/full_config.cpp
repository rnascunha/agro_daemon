#include "../coap_engine.hpp"
#include "../instance/agro.hpp"
#include "../websocket/types.hpp"
#include "process.hpp"

namespace Resource{

void put_full_config_handler(engine::message const& request,
								engine::response& response, void*,
								Agro::instance& instance,
								Agro::share_ptr data_share) noexcept
{
	CoAP::Message::Option::option op;
	Agro::Device::Device* dev;
	if(!instance.process_device_request(request, &dev, op))
	{
		//Missing host... nothing to do
		//(reality is that without host message wouldn't even get here)
		return;
	}

	std::error_code ec;
	if(!process_full_config(*dev,
					data_share,
					instance,
					response.endpoint(),
					request.payload, request.payload_len,
					ec))
	{
		tt::debug("Full config request error!");
		CoAP::Message::Option::node uri_host{op};
		response
			.code(CoAP::Message::code::bad_request)
			.add_option(uri_host)
			.payload(ec.message().c_str())
			.serialize();
		return;
	}

	instance.update_db_device(*dev);

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
