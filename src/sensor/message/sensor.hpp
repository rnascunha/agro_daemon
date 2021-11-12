#ifndef AGRO_DAEMON_SENSOR_MESSAGE_DEF_HPP__
#define AGRO_DAEMON_SENSOR_MESSAGE_DEF_HPP__

#include <string>
#include "../../message/types.hpp"
#include "../sensor_type_list.hpp"
#include "../sensor_type.hpp"
#include "../sensor_list.hpp"
#include "rapidjson/document.h"

namespace Agro{
namespace Sensor{
namespace Message{

enum class sensor_commands{
	list = 0,
	add,
	edit,
	remove,
};

constexpr const ::Message::config<sensor_commands> sensor_config[] = {
	{sensor_commands::list, "list"},
	{sensor_commands::add, "add"},
	{sensor_commands::edit, "edit"},
	{sensor_commands::remove, "remove"},
};

inline constexpr ::Message::config<sensor_commands> const* get_config(sensor_commands t) noexcept
{
	for(std::size_t i = 0; i < sizeof(sensor_config) / sizeof(sensor_config[0]); i++)
	{
		if(t == sensor_config[i].mtype) return &sensor_config[i];
	}
	return nullptr;
}

inline constexpr ::Message::config<sensor_commands> const* get_sensor_config(const char* t) noexcept
{
	for(std::size_t i = 0; i < sizeof(sensor_config) / sizeof(sensor_config[0]); i++)
	{
		if(std::strcmp(t, sensor_config[i].name) == 0) return &sensor_config[i];
	}
	return nullptr;
}

std::string sensor_types_list(Sensor_Type_List const&) noexcept;

template<typename Allocator>
void sensor_value(Agro::Sensor::sensor_value const&,
				unsigned type, unsigned index,
				Sensor_Type_List const&,
				rapidjson::Value&, Allocator&) noexcept;
template<typename Allocator>
void sensor_list(Sensor_List const& list, Sensor_Type_List const& type_list,
				rapidjson::Value& data, Allocator& alloc) noexcept;
template<typename Allocator>
void sensor_list(const void* sensor_data, std::size_t size,
				Sensor_Type_List const& type_list,
				rapidjson::Value& data, Allocator& alloc) noexcept;

}//Message
}//Sensor
}//Agro

#include "impl/sensor_impl.hpp"

#endif /* AGRO_DAEMON_SENSOR_MESSAGE_DEF_HPP__ */
