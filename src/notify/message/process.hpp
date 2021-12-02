#ifndef AGRO_DAEMON_NOTIFY_MESSAGE_PROCESS_HPP__
#define AGRO_DAEMON_NOTIFY_MESSAGE_PROCESS_HPP__

#include "rapidjson/document.h"
#include "../../instance/agro.hpp"
#include "../../websocket/types.hpp"

namespace Agro{
namespace Notify{
namespace Message{

void process(rapidjson::Document const&,
				Agro::websocket_ptr,
				Agro::instance&,
				Agro::User::Logged&) noexcept;

}//Message
}//Notify
}//Agro

#endif /* AGRO_DAEMON_NOTIFY_MESSAGE_PROCESS_HPP__ */
