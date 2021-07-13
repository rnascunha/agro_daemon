#include "types.hpp"
#include "../../websocket.hpp"

namespace Message{

static void process_ac_load(Device_List& device_list,
		mesh_addr_t const& host,
		unsigned index, bool value) noexcept
{
	auto& dev = device_list.update_ac_load(host, index, value);
	Websocket<false>::write_all(device_gpios_to_json(dev));
}

static void ac_load_response(
		engine::endpoint const&,
		mesh_addr_t const& host,
		requests req,
		CoAP::Message::message const&,
		CoAP::Message::message const& response,
		CoAP::Transmission::status_t,
		Device_List& device_list) noexcept
{
	switch(req)
	{
		case requests::ac_load1_on:
		case requests::ac_load1_off:
			process_ac_load(device_list, host, 0, static_cast<bool>(*static_cast<const uint8_t*>(response.payload) - '0'));
		break;
		case requests::ac_load2_on:
		case requests::ac_load2_off:
			process_ac_load(device_list, host, 1, static_cast<bool>(*static_cast<const uint8_t*>(response.payload) - '0'));
		break;
		case requests::ac_load3_on:
		case requests::ac_load3_off:
			process_ac_load(device_list, host, 2, static_cast<bool>(*static_cast<const uint8_t*>(response.payload) - '0'));
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

extern constexpr const request_config ac1_on = {
	requests::ac_load1_on,
	"ac1_on",
	&req_ac1_on,
	ac_load_response
};
extern constexpr const request_config ac1_off = {
	requests::ac_load1_off,
	"ac1_off",
	&req_ac1_off,
	ac_load_response
};
extern constexpr const request_config ac2_on = {
	requests::ac_load2_on,
	"ac2_on",
	&req_ac2_on,
	ac_load_response
};
extern constexpr const request_config ac2_off = {
	requests::ac_load2_off,
	"ac2_off",
	&req_ac2_off,
	ac_load_response
};
extern constexpr const request_config ac3_on = {
	requests::ac_load3_on,
	"ac3_on",
	&req_ac3_on,
	ac_load_response
};
extern constexpr const request_config ac3_off = {
	requests::ac_load3_off,
	"ac3_off",
	&req_ac3_off,
	ac_load_response
};

}//Message