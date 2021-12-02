#include "make.hpp"
#include <sstream>

namespace Agro{
namespace Notify{
namespace Message{

std::string make_device_name(Device::Device const& device) noexcept
{
	std::stringstream ss;
	if(device.name().empty())
	{
		ss << "'" << device.mac().to_string() << "'";
	}
	else
	{
		ss << "'" << device.name() << " (" << device.mac().to_string() << ")'";
	}

	return ss.str();
}

std::string make_sensor_name(Sensor::sensor_type const& type,
		Sensor::sensor_description const* sdesc) noexcept
{
	std::stringstream ss;

	if(sdesc)
	{
		ss << sdesc->long_name;
		if(type.index)
		{
			ss << "-" << type.index;
		}
	}
	else
	{
		ss <<  "Sensor-" << type.type << "-" << type.index;
	}

	return ss.str();
}

std::string make_sensor_notify_data(sensor_notify const& sn) noexcept
{
	std::stringstream ss;

	switch(sn.stype)
	{
		case sensor_type::less:
			ss << "=";
			break;
		case sensor_type::less_equal:
			ss << "<=";
			break;
		case sensor_type::equal:
			ss << "=";
			break;
		case sensor_type::greater_equal:
			ss << ">=";
			break;
		case sensor_type::greater:
			ss << ">";
			break;
		case sensor_type::different:
			ss << "!=";
			break;
	}

	ss << " " << sn.value;

	return ss.str();
}

std::string make_notify_sensor_data(Device::Device const& device,
		Sensor::sensor_type const& type,
		Sensor::sensor_description const* sdesc,
		sensor_notify const& sn,
		float value) noexcept
{
	std::stringstream ss;
	ss << "Device "
		<< make_device_name(device) << " "
		<< make_sensor_name(type, sdesc)
		<< " notify: " << value << " ("
		<< make_sensor_notify_data(sn) << ")";

	return ss.str();
}

std::string make_status_devices(std::vector<std::reference_wrapper<Device::Device const>> const& list, const char* status) noexcept
{
	std::stringstream ss;
	ss << "Devices " << status << ": ";
	for(auto const& d : list)
	{
		ss << d.get().mac().to_string() << "/";
	}
	ss.seekp(-1, std::ios::end);
	ss << ".";

	return ss.str();
}

}//Message
}//Notify
}//Agro
