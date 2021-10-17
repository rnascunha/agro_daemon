#ifndef AGRO_DAEMON_DEVICE_MESSAGE_PROCESS_HPP__
#define AGRO_DAEMON_DEVICE_MESSAGE_PROCESS_HPP__

#include "rapidjson/document.h"
#include "../../instance/agro.hpp"
#include "../../websocket/types.hpp"

namespace Agro{
namespace Device{

namespace Request{

void process_request(rapidjson::Document const& doc,
				Agro::websocket_ptr ws,
				Agro::instance& instance,
				Agro::User::Logged& user) noexcept;

}//Request

namespace Message{

void process(rapidjson::Document const&,
		Agro::websocket_ptr,
		Agro::instance&,
		Agro::User::Logged&) noexcept;

}//Message

}//Device
}//Agro

#endif /* AGRO_DAEMON_DEVICE_MESSAGE_PROCESS_HPP__ */
