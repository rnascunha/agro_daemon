#ifndef AGRO_DAEMON_SENSOR_TYPE_HPP__
#define AGRO_DAEMON_SENSOR_TYPE_HPP__

#include <cstdint>
#include <vector>

namespace Agro{
namespace Sensor{

union sensor_value{
	sensor_value(int);
	sensor_value(unsigned);
	sensor_value(float);
	sensor_value(std::uint8_t const*);

	unsigned	uint_v;
	int		 	int_v;
	float		float_v;
	std::uint8_t 	array_v[sizeof(float)];

	void set(std::uint8_t const*) noexcept;

	sensor_value& operator=(int) noexcept;
	sensor_value& operator=(unsigned) noexcept;
	sensor_value& operator=(float) noexcept;
	sensor_value& operator=(const std::uint8_t*) noexcept;
	sensor_value& operator=(sensor_value const&) noexcept;
};

struct sensor_type{
	std::uint32_t	type:24;
	std::uint32_t	index:8;
	sensor_value	value;
};

}//Sensor
}//Agro

#endif /* AGRO_DAEMON_SENSOR_TYPE_HPP__ */
