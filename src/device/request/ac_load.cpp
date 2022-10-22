#include "types.hpp"
#include "../../websocket/types.hpp"
#include "../../device/message/device.hpp"
#include "../../sensor/sensor_type.hpp"

namespace Agro{
namespace Device{
namespace Request{

static void process_ac_load(Agro::instance& instance,
		Agro::websocket_ptr ws,
		mesh_addr_t const& host,
		Sensor::sensor_type const& sensor) noexcept
{
	auto* dev = instance.device_list()[host];
	instance.update_sensor_value(*dev, sensor);

	ws->write_all_policy(Agro::Authorization::rule::view_device,
				std::make_shared<std::string>(
						Agro::Device::Message::device_sensor_data(
									*dev,
									sensor,
									instance.sensor_list())));
}

static void ac_load_response(
		engine::endpoint const&,
		mesh_addr_t const& host,
		request_type req,
		CoAP::Message::message const&,
		CoAP::Message::message const& response,
		CoAP::Transmission::status_t,
		Agro::instance& instance,
		Agro::websocket_ptr ws) noexcept
{
	if(response.payload_len != sizeof(Sensor::sensor_type))
	{
		tt::error("AC response packet wrong size [%zu]", response.payload_len);
		return;
	}

	switch(req)
	{
		case request_type::ac_load1_on:
		case request_type::ac_load1_off:
		case request_type::ac_load2_on:
		case request_type::ac_load2_off:
		case request_type::ac_load3_on:
		case request_type::ac_load3_off:
			process_ac_load(instance, ws, host, *static_cast<Sensor::sensor_type const*>(response.payload));
		break;
		default:
			break;
	}
}

#define AC_LOAD(name,num,value) static request_message const req_##name = { \
	CoAP::Message::code::put, 									\
	{ 															\
		{CoAP::Message::Option::code::uri_path, "ac_load"}, 	\
		{CoAP::Message::Option::code::uri_path, #num}, 			\
		{CoAP::Message::Option::code::uri_query, "value=" value}, 	\
	} 															\
}

AC_LOAD(ac1_on, 1, "1");
AC_LOAD(ac1_off, 1, "0");
AC_LOAD(ac2_on, 2, "1");
AC_LOAD(ac2_off, 2, "0");
AC_LOAD(ac3_on, 3, "1");
AC_LOAD(ac3_off, 3, "0");

const request_config ac1_on = {
	{request_type::ac_load1_on, "ac1_on"},
	&req_ac1_on,
	ac_load_response
};
const request_config ac1_off = {
	{request_type::ac_load1_off, "ac1_off"},
	&req_ac1_off,
	ac_load_response
};
const request_config ac2_on = {
	{request_type::ac_load2_on, "ac2_on"},
	&req_ac2_on,
	ac_load_response
};
const request_config ac2_off = {
	{request_type::ac_load2_off, "ac2_off"},
	&req_ac2_off,
	ac_load_response
};
const request_config ac3_on = {
	{request_type::ac_load3_on, "ac3_on"},
	&req_ac3_on,
	ac_load_response
};
const request_config ac3_off = {
	{request_type::ac_load3_off, "ac3_off"},
	&req_ac3_off,
	ac_load_response
};

}//Request
}//Device
}//Agro
