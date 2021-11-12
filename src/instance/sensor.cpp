#include "agro.hpp"

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

std::size_t instance::update_sensor_value(const Device::Device& device, const void* data, std::size_t size) noexcept
{
	update_db_device(device);
	return db_.update_sensor_value(device, data, size);
}

bool instance::update_sensor_value(const Device::Device& device, Sensor::sensor_type const& value) noexcept
{
	return db_.update_sensor_value(device, value) != SQLITE_DONE;
}

}//Agro
