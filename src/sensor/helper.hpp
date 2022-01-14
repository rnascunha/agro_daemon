#ifndef AGRO_DAEMON_SENSOR_HELPER_HPP__
#define AGRO_DAEMON_SENSOR_HELPER_HPP__

#include "sensor_type_list.hpp"
#include "sensor_type.hpp"

namespace Agro{
namespace Sensor{

float get_sensor_value(sensor_description const*, sensor_value const&) noexcept;

}//Sensor
}//Agro

#endif /* AGRO_DAEMON_SENSOR_HELPER_HPP__ */
