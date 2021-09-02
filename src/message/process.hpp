#ifndef AGRO_MESH_MESSAGE_PROCESS_HPP__
#define AGRO_MESH_MESSAGE_PROCESS_HPP__

#include "../coap_engine.hpp"
#include "../device/list.hpp"
#include "../agro.hpp"
#include "../user/user.hpp"
#include "../websocket/websocket.hpp"
#include <string>

namespace Message{

void process_request(rapidjson::Document const& doc,
			Device_List& device_list,
			engine& coap_engine) noexcept;
void process_commands(rapidjson::Document const& d, Device_List& list) noexcept;
void process(std::string&& str,
		std::shared_ptr<Websocket<false>>,
		Agro::instance&,
		Agro::User&) noexcept;

}//Message

#endif /* AGRO_MESH_MESSAGE_PROCESS_HPP__ */
