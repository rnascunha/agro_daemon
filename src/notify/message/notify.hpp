#ifndef AGRO_DAEMON_NOTIFY_MESSAGE_HPP__
#define AGRO_DAEMON_NOTIFY_MESSAGE_HPP__

#include "../../message/types.hpp"

namespace Agro{
namespace Notify{
namespace Message{

enum class notify_commands{
	general_list,
	general_set,
	device_list,
	device_set,
	sensor_list,
	sensor_set,
};

constexpr const ::Message::config<notify_commands> notify_config[] = {
	{notify_commands::general_list, "general_list"},
	{notify_commands::general_set, "general_set"},
	{notify_commands::device_list, "device_list"},
	{notify_commands::device_set, "device_set"},
	{notify_commands::sensor_list, "sensor_list"},
	{notify_commands::sensor_set, "sensor_set"},
};

inline constexpr ::Message::config<notify_commands> const* get_config(notify_commands t) noexcept
{
	for(std::size_t i = 0; i < sizeof(notify_config) / sizeof(notify_config[0]); i++)
	{
		if(t == notify_config[i].mtype) return &notify_config[i];
	}
	return nullptr;
}

inline constexpr ::Message::config<notify_commands> const* get_notify_config(const char* t) noexcept
{
	for(std::size_t i = 0; i < sizeof(notify_config) / sizeof(notify_config[0]); i++)
	{
		if(std::strcmp(t, notify_config[i].name) == 0) return &notify_config[i];
	}
	return nullptr;
}

}//Message
}//Notify
}//Agro

#endif /* AGRO_DAEMON_NOTIFY_MESSAGE_HPP__ */
