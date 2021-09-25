#ifndef AGRO_DAEMON_DEVICE_MESSAGE_PROCESS_HPP__
#define AGRO_DAEMON_DEVICE_MESSAGE_PROCESS_HPP__

#include "rapidjson/document.h"
#include "../../websocket/types.hpp"

namespace Agro{
namespace Device{
namespace Message{

void process(rapidjson::Document const&,
		Agro::websocket_ptr,
		Agro::instance&,
		Agro::User::Logged&) noexcept;

}//Message
}//Device
}//Agro

#endif /* AGRO_DAEMON_DEVICE_MESSAGE_PROCESS_HPP__ */
