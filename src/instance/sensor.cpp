#include "agro.hpp"
#include "../helper/time_helper.hpp"

namespace Agro{

bool instance::remove_sensor_type(std::string const& name) noexcept
{
	int rc = db_.remove_sensor_type(name);
	if(rc != SQLITE_DONE)
	{
		tt::error("Error removing sensor type '%s' from DB [%d]", name.c_str(), rc);
		return false;
	}

	if(!sensor_list_.remove(name))
	{
		tt::error("Error removing sensor type '%s'", name.c_str());
		return false;
	}

	return true;
}

bool instance::add_sensor_type(Sensor::sensor_description const& sensor) noexcept
{
	int rc = db_.add_sensor_type(sensor);
	if(rc != SQLITE_DONE)
	{
		tt::error("Error adding sensor type '%s' from DB [%d]", sensor.name.c_str(), rc);
		return false;
	}

	if(!sensor_list_.add(sensor))
	{
		tt::error("Error removing sensor type '%s'", sensor.name.c_str());
		return false;
	}

	return true;
}

bool instance::update_sensor_type(Sensor::sensor_description const& sensor) noexcept
{
	int rc = db_.update_sensor_type(sensor);
	if(rc != SQLITE_DONE)
	{
		tt::error("Error editing sensor type '%s' from DB [%d]", sensor.name.c_str(), rc);
		return false;
	}

	if(!sensor_list_.update(sensor))
	{
		tt::error("Error editing sensor type '%s'", sensor.name.c_str());
		return false;
	}

	return true;
}

std::size_t instance::update_sensor_value(Device::Device& device,
		const void* data, std::size_t size) noexcept
{
	const std::uint8_t* d8u = static_cast<const std::uint8_t*>(data);

	std::size_t dsize = 0, count = 0;
	while((dsize + sizeof(Sensor::sensor_type)) <= size)
	{
		Sensor::sensor_type const* sensor = reinterpret_cast<Sensor::sensor_type const*>(d8u + dsize);
		if(update_sensor_value(device, *sensor)) count++;
		dsize += sizeof(Sensor::sensor_type);
	}

	return count;
}

bool instance::update_sensor_value(Device::Device& device, Sensor::sensor_type const& sensor) noexcept
{
	value_time time = get_time();
	Sensor::sensor_description const* stype = sensor_list_.get(sensor.type);
	bool added = device.update_sensor(sensor.type, sensor.index, time,
									sensor.value, stype ? stype->add_change : false);
	if(added)
	{
		db_.update_sensor_value(device, sensor, time);
	}

	notify_all_policy(Authorization::rule::view_device,
			device,
			sensor,
			stype);

	return added;
}

std::string instance::get_sensor_value(
		Device::device_id id,
		unsigned type, unsigned index,
		value_time init, value_time end) noexcept
{
	Sensor::sensor_description const* stype = sensor_list_.get(type);

	return db_.get_sensor_value(id, stype, type, index, init, end);
}

}//Agro
