#include "sensor_list.hpp"

namespace Agro{
namespace Sensor{

std::size_t Sensor_List::add(const void* data, std::size_t size) noexcept
{
	const std::uint8_t* d8u = static_cast<const std::uint8_t*>(data);
	std::size_t dsize = 0, count = 0;
	Sensor::sensor_type const* sensor;

	while(dsize + sizeof(sensor_type) <= size)
	{
		sensor = reinterpret_cast<Sensor::sensor_type const*>(d8u + dsize);
		add(sensor->type, sensor->index, sensor->value);

		dsize += sizeof(sensor_type);
		count++;
	}

	return count;
}

void Sensor_List::add(unsigned type, unsigned index,
				sensor_value const& svalue) noexcept
{
	auto const lkey = std::make_pair(type, index);

	auto value_list = list_.find(lkey);
	if(value_list != list_.end())
	{
		value_list->second.add(svalue.int_v);
	}
	else
	{
		//Creating list, referering to it and addind the value
		list_.emplace(lkey, value{}).first->second.add(svalue.int_v);
	}
}

void Sensor_List::add(unsigned type, unsigned index,
				value_time time, sensor_value const& svalue) noexcept
{
	auto const lkey = std::make_pair(type, index);

	auto value_list = list_.find(lkey);
	if(value_list != list_.end())
	{
		value_list->second.add(time, svalue.int_v);
	}
	else
	{
		//Creating list, referering to it and addind the value
		list_.emplace(lkey, value{}).first->second.add(time, svalue.int_v);
	}
}

Sensor_List::value& Sensor_List::get_or_add(unsigned type, unsigned index) noexcept
{
	auto const lkey = std::make_pair(type, index);

	auto value_list = list_.find(lkey);
	if(value_list != list_.end())
	{
		return value_list->second;
	}
	//Creating list, referering to it and addind the value
	return list_.emplace(lkey, value{}).first->second;
}

}//Sensor
}//Agro
