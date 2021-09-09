#ifndef AGRO_MESH_MESSAGE_PROCESS_HPP__
#define AGRO_MESH_MESSAGE_PROCESS_HPP__

#include "../coap_engine.hpp"
#include "../device/list.hpp"
#include "../agro.hpp"
#include "../user/user.hpp"
#include "../websocket/types.hpp"
#include <string>

namespace Message{

void process_request(rapidjson::Document const& doc,
		Agro::websocket_ptr ws,
			Device_List& device_list,
			engine& coap_engine) noexcept;
void process_commands(rapidjson::Document const& d,
		Agro::websocket_ptr ws,
			Device_List& list) noexcept;
void process(std::string&& str,
		Agro::websocket_ptr,
		Agro::instance const&,
		Agro::User&) noexcept;

}//Message

#endif /* AGRO_MESH_MESSAGE_PROCESS_HPP__ */
