#ifndef AGRO_DAEMON_NOTIFY_ALERT_HPP__
#define AGRO_DAEMON_NOTIFY_ALERT_HPP__

namespace Agro{

enum class alert{
	//general
	no_alert = 0,
	//user
	login = 1 << 0,
	logout = 1 << 1,
	add_user = 1 << 2,
	user_removed = 1 << 3,
	//device
	add_device = 1 << 4,			//New device added
	device_connected = 1 << 5,
	device_disconnected = 1 << 6,
	device_image_updated = 1 << 7,
	device_app_installed = 1 << 8,
	//sensor
	sensor_value = 1 << 9,
	sensor_value_change = 1 << 10,
	sensor_value_out_of_bound = 1 << 11,
	//Image
	image_uploaded = 1 << 12,
	//App
	app_uploaded = 1 << 13
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
