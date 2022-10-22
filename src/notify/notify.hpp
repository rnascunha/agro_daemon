#ifndef AGRO_DAEMON_NOTIFY_NOTIFY_TYPE_HPP__
#define AGRO_DAEMON_NOTIFY_NOTIFY_TYPE_HPP__

#include <map>
#include <vector>
#include <limits>

#include "../device/device.hpp"
#include "../sensor/sensor_type.hpp"

namespace Agro{
namespace Notify{

enum class general_type{
	//general
	no_alert = 0,
	//device
	device_new = 1 << 0,
	device_connect = 1 << 1,
	device_disconnect = 1 << 2,
	device_updated = 1 << 3,
	device_app_instaled = 1 << 4,
	//Image
	image_add = 1 << 5,
	image_delete = 1 << 6,
	//App
	app_add = 1 << 7,
	app_delete = 1 << 8,
	//Jobs
	job_running = 1 << 9,
	job_stopped = 1 << 10,
	//all
	all_alerts = device_new | device_connect | device_disconnect | device_updated | device_app_instaled |
				image_add | image_delete |
				app_add | app_delete |
				job_running | job_stopped
};

enum class device_type{
	no_alert = 0,
	connect = 1 << 0,
	disconnect = 1 << 1,
	updated = 1 << 2,
	app_installed = 1 << 3
};

enum class sensor_type{
	less = 0,
	less_equal,
	equal,
	greater_equal,
	greater,
	different
};

struct sensor_notify{
	sensor_notify(
			bool,
			sensor_type,
			float,
			bool);
	sensor_notify(sensor_notify const&) = default;

	bool enabled;
	sensor_type	stype;
	float value;
	bool status;
};

struct sensor_temp_notify{
	sensor_temp_notify(
			unsigned,
			unsigned,
			bool,
			sensor_type,
			float,
			bool);

	unsigned type;
	unsigned index;
	bool enabled;
	sensor_type	stype;
	float value;
	bool status;
};

class Notify{
	public:
		using device_container = std::map<Device::device_id, device_type>;

		using sensor_container_key = std::tuple<Device::device_id, unsigned, unsigned>;
		using sensor_container_value = std::vector<sensor_notify>;
		using sensor_container = std::map<sensor_container_key, sensor_container_value>;

		Notify();

		//General
		void set(general_type) noexcept;
		general_type get() const noexcept;

		bool can(general_type) const noexcept;

		//Device
		void set(Device::device_id, device_type) noexcept;
		device_container const&
		get_devices() const noexcept;

		bool can(Device::device_id, device_type) const noexcept;

		//Sensor
		void set(Device::device_id,
				std::vector<sensor_temp_notify> const& value) noexcept;
		sensor_container const& get_sensors() const noexcept;
		void add(Device::device_id, unsigned type, unsigned index,
				bool enabled, sensor_type, float, bool status = false) noexcept;

		sensor_container_value const* get_device_sensors(
				Device::device_id, unsigned type, unsigned index) const noexcept;

		std::vector<sensor_notify> can(Device::device_id id,
				unsigned type, unsigned index,
				float value) noexcept;
	private:
		general_type type_ = general_type::all_alerts;
		device_container dev_type_;
		sensor_container sensor_type_;
};

}//Notify
}//Agro

//#include "impl/notify_impl.hpp"

#endif /* AGRO_DAEMON_NOTIFY_NOTIFY_TYPE_HPP__ */
