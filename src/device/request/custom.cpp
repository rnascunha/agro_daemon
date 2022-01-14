#include "types.hpp"
#include "../../message/make.hpp"
#include "../../websocket/types.hpp"

namespace Agro{
namespace Device{
namespace Request{

static void custom_response(
		engine::endpoint const& ep,
		mesh_addr_t const& host,
		type,
		CoAP::Message::message const& request,
		CoAP::Message::message const& response,
		CoAP::Transmission::status_t status,
		Agro::instance&,
		Agro::websocket_ptr ws) noexcept
{
	ws->write(Message::device_custom_response(request, response, ep, status));
}

extern constexpr const request_config custom = {
		type::custom,
		"custom",
		{},
		custom_response
};

}//Request
}//Device
}//Agro
