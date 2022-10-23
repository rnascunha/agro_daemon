#ifndef AGRO_DAEMON_NOTIFY_MESSAGE_MAKE_HPP__
#define AGRO_DAEMON_NOTIFY_MESSAGE_MAKE_HPP__

//#include "../../message/types.hpp"
#include "../notify.hpp"
#include "../../sensor/sensor_type_list.hpp"
#include "../factory.hpp"
#include "../../helper/utility.hpp"

#include <string>

namespace Agro{
namespace Notify{
namespace Message{

constexpr const config<general_type> general_type_config[] = {
	{general_type::no_alert, "no_alert"},
	{general_type::device_new, "new_device"},
	{general_type::device_connect, "device_conn"},
	{general_type::device_disconnect, "device_dis"},
	{general_type::device_updated, "device_update"},
	{general_type::device_app_instaled, "device_app_installed"},
	{general_type::image_add, "image_add"},
	{general_type::image_delete, "image_del"},
	{general_type::app_add, "app_add"},
	{general_type::app_delete, "add_del"},
	{general_type::job_running, "job_run"},
	{general_type::job_stopped, "job_stop"}
};

inline constexpr auto get_general_type_config(const char* name) noexcept
{
    return ::get_config(name, general_type_config);
}

inline constexpr auto get_config(general_type mtype) noexcept
{
    return ::get_config(mtype, general_type_config);
}

constexpr const config<device_type> device_type_config[] = {
	{device_type::no_alert, "no_alert"},
	{device_type::connect, "connect"},
	{device_type::disconnect, "disconnect"},
	{device_type::updated, "updated"},
	{device_type::app_installed, "app_installed"},
};

inline constexpr auto get_device_type_config(const char* name) noexcept
{
    return ::get_config(name, device_type_config);
}

inline constexpr auto get_config(device_type mtype) noexcept
{
    return ::get_config(mtype, device_type_config);
}

constexpr const config<sensor_type> sensor_type_config[] = {
	{sensor_type::less, "less"},
	{sensor_type::less_equal, "less_equal"},
	{sensor_type::equal, "equal"},
	{sensor_type::greater_equal, "greater_equal"},
	{sensor_type::greater, "greater"},
	{sensor_type::different, "different"},
};

inline constexpr auto get_sensor_type_config(const char* name) noexcept
{
    return ::get_config(name, sensor_type_config);
}

inline constexpr auto get_config(sensor_type mtype) noexcept
{
    return ::get_config(mtype, sensor_type_config);
}

std::string make_list(Notify const&) noexcept;
std::string make_device_list(Notify const&) noexcept;
std::string make_sensor_list(Notify const&) noexcept;
std::string make_sensor_device_list(Device::device_id, Notify const&) noexcept;
std::string make_public_key(std::string_view const& public_key) noexcept;

//Send notify message
std::string make_device_name(Device::Device const& device) noexcept;
std::string make_sensor_name(Sensor::sensor_type const& type,
		Sensor::sensor_description const* sdesc) noexcept;
std::string make_sensor_notify_data(sensor_notify const& sn) noexcept;
std::string make_notify_sensor_data(Device::Device const& device,
		Sensor::sensor_type const& type,
		Sensor::sensor_description const* sdesc,
		sensor_notify const& sn,
		float value) noexcept;

std::string make_status_devices(
		std::vector<std::reference_wrapper<Device::Device const>> const&,
		const char*) noexcept;

std::string make_credential_list(Factory const&) noexcept;

}//Message
}//Notify
}//Agro

#endif /* AGRO_DAEMON_NOTIFY_MESSAGE_MAKE_HPP__ */
