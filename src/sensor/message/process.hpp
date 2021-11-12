#ifndef AGRO_DAEMON_SENSOR_MESSAGE_PROCESS_HPP__
#define AGRO_DAEMON_SENSOR_MESSAGE_PROCESS_HPP__

#include "../../instance/agro.hpp"
#include "../../user/user.hpp"
#include "../../websocket/types.hpp"

namespace Agro{
namespace Sensor{
namespace Message{

void process(rapidjson::Document const&,
		websocket_ptr,
		instance&,
		User::Logged&) noexcept;

}//Message
}//Sensor
}//Agro

#endif /* AGRO_DAEMON_SENSOR_MESSAGE_PROCESS_HPP__ */
