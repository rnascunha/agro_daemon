#include "sensor_type.hpp"
#include <string.h>

namespace Agro{
namespace Sensor{

sensor_value::sensor_value(int v)
	: int_v{v}{}
sensor_value::sensor_value(unsigned v)
	: uint_v{v}{}
sensor_value::sensor_value(float v)
	: float_v{v}{}
sensor_value::sensor_value(uint8_t const* v)
{
	set(v);
}
sensor_value::sensor_value(sensor_value const& v)
	: sensor_value(v.array_v){}

sensor_value& sensor_value::operator=(int v) noexcept
{
	int_v = v;
	return *this;
}

sensor_value& sensor_value::operator=(unsigned v) noexcept
{
	uint_v = v;
	return *this;
}

sensor_value& sensor_value::operator=(float v) noexcept
{
	float_v = v;
	return *this;
}

sensor_value& sensor_value::operator=(const std::uint8_t* v) noexcept
{
	set(v);
	return *this;
}

sensor_value& sensor_value::operator=(sensor_value const& v) noexcept
{
	this->set(v.array_v);
	return *this;
}

void sensor_value::set(uint8_t const* v) noexcept
{
	memcpy(array_v, v, sizeof(float));
}

bool sensor_value::operator==(int rhs) const noexcept
{
	return int_v == rhs;
}

bool sensor_value::operator==(unsigned rhs) const noexcept
{
	return uint_v == rhs;
}

bool sensor_value::operator==(float rhs) const noexcept
{
	return float_v == rhs;
}

bool sensor_value::operator!=(int rhs) const noexcept
{
	return int_v != rhs;
}

bool sensor_value::operator!=(unsigned rhs) const noexcept
{
	return uint_v != rhs;
}

bool sensor_value::operator!=(float rhs) const noexcept
{
	return float_v != rhs;
}

bool sensor_value::operator<(int rhs) const noexcept
{
	return int_v < rhs;
}

bool sensor_value::operator<(unsigned rhs) const noexcept
{
	return uint_v < rhs;
}

bool sensor_value::operator<(float rhs) const noexcept
{
	return float_v < rhs;
}

bool sensor_value::operator<=(int rhs) const noexcept
{
	return int_v <= rhs;
}

bool sensor_value::operator<=(unsigned rhs) const noexcept
{
	return uint_v <= rhs;
}

bool sensor_value::operator<=(float rhs) const noexcept
{
	return float_v <= rhs;
}

bool sensor_value::operator>(int rhs) const noexcept
{
	return int_v > rhs;
}

bool sensor_value::operator>(unsigned rhs) const noexcept
{
	return uint_v > rhs;
}

bool sensor_value::operator>(float rhs) const noexcept
{
	return float_v > rhs;
}

bool sensor_value::operator>=(int rhs) const noexcept
{
	return int_v >= rhs;
}

bool sensor_value::operator>=(unsigned rhs) const noexcept
{
	return uint_v >= rhs;
}

bool sensor_value::operator>=(float rhs) const noexcept
{
	return float_v >= rhs;
}

}//Sensor
}//Agro
