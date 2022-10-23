#ifndef AGRO_DAEMON_NOTIFY_MESSAGE_HPP__
#define AGRO_DAEMON_NOTIFY_MESSAGE_HPP__

//#include "../../message/types.hpp"
#include "../../helper/utility.hpp"

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
	credential_list,
	update_credential,
};

constexpr const config<notify_commands> notify_config[] = {
	{notify_commands::general_list, "general_list"},
	{notify_commands::general_set, "general_set"},
	{notify_commands::device_list, "device_list"},
	{notify_commands::device_set, "device_set"},
	{notify_commands::sensor_list, "sensor_list"},
	{notify_commands::sensor_set, "sensor_set"},
	{notify_commands::credential_list, "credential_list"},
	{notify_commands::update_credential, "update_credential"},
};

inline constexpr auto get_notify_config(const char* name) noexcept
{
    return ::get_config(name, notify_config);
}

inline constexpr auto get_config(notify_commands mtype) noexcept
{
    return ::get_config(mtype, notify_config);
}

}//Message
}//Notify
}//Agro

#endif /* AGRO_DAEMON_NOTIFY_MESSAGE_HPP__ */
