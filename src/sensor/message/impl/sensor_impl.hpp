#ifndef AGRO_DAEMON_SENSOR_MESSAGE_DEF_IMPL_HPP__
#define AGRO_DAEMON_SENSOR_MESSAGE_DEF_IMPL_HPP__

#include "../sensor.hpp"
#include "../../../helper/time_helper.hpp"
#include "../../../message/make.hpp"

namespace Agro{
namespace Sensor{
namespace Message{

template<typename Allocator>
static void add_value(rapidjson::Value& data,
		sensor_description const* desc,
		Agro::Sensor::sensor_value const& value,
		Allocator& alloc) noexcept
{
	if(!desc)
	{
		data.AddMember("value", value.int_v, alloc);
	}
	else {
		switch(desc->type)
		{
			case sensor_unit_type::tunsigned:
				data.AddMember("value", value.uint_v, alloc);
				break;
			case sensor_unit_type::tfloat:
				data.AddMember("value", value.float_v, alloc);
				break;
			case sensor_unit_type::array:
				break;
			case sensor_unit_type::integer:
			default:
				data.AddMember("value", value.int_v, alloc);
				break;
		}
	}
}

template<typename Allocator>
static void add_value_list(rapidjson::Value& data,
		sensor_description const* desc,
		Sensor_List::value const& value_list,
		Allocator& alloc) noexcept
{
	data.SetArray();
	for(auto const& [time, value] : value_list)
	{
		rapidjson::Value v;
		v.SetObject();

		v.AddMember("time", time, alloc);
		Agro::Sensor::sensor_value vv{value};
		add_value(v, desc, vv, alloc);

		data.PushBack(v, alloc);
	}
}

template<typename Allocator>
void sensor_value(Agro::Sensor::sensor_value const& value,
				unsigned type, unsigned index,
				Sensor_Type_List const& type_list,
				rapidjson::Value& data, Allocator& alloc) noexcept
{
	data.SetObject();
	data.AddMember("type", type, alloc);
	data.AddMember("index", index, alloc);

	rapidjson::Value sdata;
	sdata.SetObject();
	sdata.AddMember("time", get_time(), alloc);
	add_value(sdata, type_list.get(type), value, alloc);

	data.AddMember("data", sdata, alloc);
}

template<typename Allocator>
void sensor_value(Sensor_List::value const& value,
				unsigned type, unsigned index,
				Sensor_Type_List const& type_list,
				rapidjson::Value& data, Allocator& alloc) noexcept
{
	data.SetObject();
	data.AddMember("type", type, alloc);
	data.AddMember("index", index, alloc);

	rapidjson::Value sdata;
	add_value_list(sdata, type_list.get(type), value, alloc);

	data.AddMember("data", sdata, alloc);
}

template<typename Allocator>
void sensor_list(Sensor_List const& list, Sensor_Type_List const& type_list,
				rapidjson::Value& data, Allocator& alloc) noexcept
{
	data.SetArray();
	for(auto const& [key, value] : list)
	{
		rapidjson::Value sdata;
		sensor_value(value, key.first, key.second, type_list, sdata, alloc);
		data.PushBack(sdata, alloc);
	}
}

template<typename Allocator>
void sensor_list(const void* sensor_data, std::size_t size,
				Sensor_Type_List const& type_list,
				rapidjson::Value& data, Allocator& alloc) noexcept
{
	data.SetArray();

	const std::uint8_t* d8u = static_cast<const std::uint8_t*>(sensor_data);
	std::size_t dsize = 0;
	while((dsize + sizeof(Sensor::sensor_type)) <= size)
	{
		Sensor::sensor_type const* sso = reinterpret_cast<Sensor::sensor_type const*>(d8u + dsize);

		rapidjson::Value sdata;
		sensor_value(sso->value, sso->type, sso->index, type_list, sdata, alloc);
		data.PushBack(sdata, alloc);

		dsize += sizeof(Sensor::sensor_type);
	}
}

}//Message
}//Sensor
}//Agro

#endif /* AGRO_DAEMON_SENSOR_MESSAGE_DEF_IMPL_HPP__ */
