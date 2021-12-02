#include "helper.hpp"

namespace Agro{
namespace Sensor{

float get_sensor_value(sensor_description const* sd, sensor_value const& sv) noexcept
{
	if(!sd) return static_cast<float>(sv.int_v);

	switch(sd->type)
	{
		case sensor_unit_type::tunsigned:
			return static_cast<float>(sv.uint_v);
		case sensor_unit_type::tfloat:
			return sv.float_v;
		case sensor_unit_type::integer:
		default:
			break;
	}
	return static_cast<float>(sv.int_v);
}

}//Sensor
}//Agro
