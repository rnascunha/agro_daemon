#ifndef AGRO_DAEMON_NOTIFY_ALERT_HPP__
#define AGRO_DAEMON_NOTIFY_ALERT_HPP__

namespace Agro{

enum class alert{
	//general
	no_alert = 0,
	//user
	login = 1 << 0,
	logout = 1 << 1,
	//device
	add_device = 1 << 2,			//New device added
	device_connected = 1 << 3,
	device_disconnected = 1 << 4,
	//sensor
	sensor_value = 1 << 5,
	sensor_value_change = 1 << 6,
	sensor_value_out_of_bound = 1 << 7
};

constexpr alert operator|(alert lhs, alert rhs) noexcept
{
	return static_cast<alert>(static_cast<int>(lhs) | static_cast<int>(rhs));
}

constexpr bool operator&(alert lhs, alert rhs) noexcept
{
	return static_cast<bool>(static_cast<int>(lhs) & static_cast<int>(rhs));
}

static constexpr const alert all_user_alert = alert::login
												| alert::logout;
static constexpr const alert all_device_alert = alert::add_device
												| alert::device_connected
												| alert::device_disconnected;
static constexpr const alert all_sensor_alert = alert::sensor_value
												| alert::sensor_value_change
												| alert::sensor_value_out_of_bound;

}//Agro

#endif /* AGRO_DAEMON_NOTIFY_ALERT_HPP__ */
