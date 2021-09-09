#include "../coap_engine.hpp"
#include "../device/list.hpp"
#include "../websocket/types.hpp"
#include "process.hpp"
#include <cstdio>

namespace Resource{

void put_sensor_data_handler(engine::message const& request,
								engine::response& response, void*,
								Device_List& device_list,
								Agro::share_ptr data_share) noexcept
{
	printf("Resource Sensor data handler\n");

	CoAP::Message::Option::option op;
	CoAP::Message::Option::get_option(request, op, CoAP::Message::Option::code::uri_host);

	std::error_code ec;
	if(!process_sensor_data(device_list,
					data_share,
					response.endpoint(),
					op,
					request.payload, request.payload_len,
					ec))
	{
		CoAP::Message::Option::node uri_host{op};
		response
			.code(CoAP::Message::code::bad_request)
			.add_option(uri_host)
			.payload(ec.message().c_str())
			.serialize();
		return;
	}

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
