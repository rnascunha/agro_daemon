#include "types.hpp"
#include "../../websocket/types.hpp"

namespace Message{

static void custom_response(
		engine::endpoint const& ep,
		mesh_addr_t const& host,
		requests,
		CoAP::Message::message const& request,
		CoAP::Message::message const& response,
		CoAP::Transmission::status_t status,
		Device_List& device_list,
		Agro::websocket_ptr ws) noexcept
{
	rapidjson::Document doc;
	Message::make_response(doc, request, response, ep, status);
	ws->write_all(Message::stringify(doc));
}

extern constexpr const request_config custom = {
		requests::custom,
		"custom",
		{},
		custom_response
};

}//Message
