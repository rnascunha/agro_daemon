#include "types.hpp"
#include "../../websocket/types.hpp"
#include "../message/device.hpp"
#include "../message/esp_reset_reason.hpp"

namespace Agro{
namespace Device{
namespace Request{

static request_message const req_waive_root = {
	CoAP::Message::code::put,
	{
		{CoAP::Message::Option::code::uri_path, "net"},
		{CoAP::Message::Option::code::uri_path, "waive"}
	},
};

const request_config waive_root = {
	{request_type::waive_root, "waive_root"},
	&req_waive_root,
};

}//Request
}//Device
}//Agro
