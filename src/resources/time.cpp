#include "../coap_engine.hpp"
#include "../device/types.hpp"

namespace Resource{

void get_time_handler(engine::message const& request,
								engine::response& response, void*) noexcept
{
	printf("Resource get time handler\n");

	CoAP::Message::Option::option op;
	if(!CoAP::Message::Option::get_option(request, op, CoAP::Message::Option::code::uri_host))
	{
		printf("TIME RESOURCE error getting host...\n");
	}
	else
	{
		printf("Received: %.*s\n", op.length, static_cast<const char*>(op.value));
	}

	CoAP::Message::content_format format{CoAP::Message::content_format::application_octet_stream};
	CoAP::Message::Option::node content{format};

	value_time time = get_time();

	CoAP::Message::Option::node uri_host{op};
	response
		.code(CoAP::Message::code::content)
		.add_option(content)
		.add_option(uri_host)
		.payload(&time, sizeof(value_time))
		.serialize();
}

}//Resource
