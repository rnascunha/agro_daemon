#ifndef AGRO_MESH_MESSAGE_PROCESS_HPP__
#define AGRO_MESH_MESSAGE_PROCESS_HPP__

#include "../instance/agro.hpp"
#include "../coap_engine.hpp"
#include "../device/list.hpp"
#include "../user/user.hpp"
#include "../websocket/types.hpp"
#include <string>

namespace Message{

void process_request(rapidjson::Document const&,
				Agro::websocket_ptr,
				Agro::instance&,
				Agro::User::Logged&) noexcept;
void process(std::string&& str,
		Agro::websocket_ptr,
		Agro::instance&,
		Agro::User::Logged&) noexcept;

}//Message

#endif /* AGRO_MESH_MESSAGE_PROCESS_HPP__ */
