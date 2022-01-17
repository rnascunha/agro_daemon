#ifndef AGRO_DAEMON_SENSOR_MESSAGE_DEF_HPP__
#define AGRO_DAEMON_SENSOR_MESSAGE_DEF_HPP__

#include <string>
#include "../sensor_type_list.hpp"
#include "../sensor_type.hpp"
#include "../sensor_list.hpp"
#include "rapidjson/document.h"
#include "../../helper/utility.hpp"

namespace Agro{
namespace Sensor{
namespace Message{

enum class sensor_commands{
	list = 0,
	add,
	edit,
	remove,
	exportv
};

constexpr const config<sensor_commands> sensor_config[] = {
	{sensor_commands::list, "list"},
	{sensor_commands::add, "add"},
	{sensor_commands::edit, "edit"},
	{sensor_commands::remove, "remove"},
	{sensor_commands::exportv, "export"},
};

inline constexpr auto get_sensor_config(const char* name) noexcept
{
    return ::get_config(name, sensor_config);
}

inline constexpr auto get_config(sensor_commands mtype) noexcept
{
    return ::get_config(mtype, sensor_config);
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
