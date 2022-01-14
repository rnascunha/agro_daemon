#include "sensor_type_list.hpp"

namespace Agro{
namespace Sensor{

sensor_description* Sensor_Type_List::add(sensor_description const& sensor) noexcept
{
	auto u = list_.find(sensor.id);
	if(u != list_.end())
	{
		return nullptr;
	}

	return &list_.emplace(sensor.id, sensor).first->second;
}

bool Sensor_Type_List::update(sensor_description const& sensor) noexcept
{
	auto u = list_.find(sensor.id);
	if(u == list_.end())
	{
		return false;
	}

	u->second.long_name = sensor.long_name;
	u->second.unit = sensor.unit;
	u->second.unit_name = sensor.unit_name;
	u->second.description = sensor.description;
	u->second.add_change = sensor.add_change;

	return true;
}

bool Sensor_Type_List::remove(std::string const& name) noexcept
{
	sensor_description const* s = nullptr;
	for(auto const& [id, sensor] : list_)
	{
		if(sensor.name == name)
		{
			s = &sensor;
			break;
		}
	}
	if(!s)
	{
		return false;
	}
	list_.erase(s->id);

	return true;
}

bool Sensor_Type_List::remove(sensor_id id) noexcept
{
	auto u = list_.find(id);
	if(u == list_.end())
	{
		return false;
	}
	list_.erase(id);

	return true;
}

sensor_description const* Sensor_Type_List::get(sensor_id id) const noexcept
{
	auto u = list_.find(id);
	if(u == list_.end())
	{
		return nullptr;
	}
	return &u->second;
}


}//Sensor
}//Agro
