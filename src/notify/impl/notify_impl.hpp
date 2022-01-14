#ifndef AGRO_DAEMON_NOTIFY_NOTIFY_TYPE_IMPL_HPP__
#define AGRO_DAEMON_NOTIFY_NOTIFY_TYPE_IMPL_HPP__

//#include "../notify.hpp"

namespace Agro{
namespace Notify{

static bool compare(sensor_type type, float svalue, float value)
{
	switch(type)
	{
		case sensor_type::less:
			return value < svalue;
		case sensor_type::less_equal:
			return value <= svalue;
		case sensor_type::equal:
			return value == svalue;
		case sensor_type::greater_equal:
			return value > svalue;
		case sensor_type::greater:
			return value >= svalue;
		case sensor_type::different:
			return value != svalue;
		default:
		break;
	}
	return false;
}

//template<typename Value>
//bool Notify::can(Device::device_id id,
//		unsigned type, unsigned index,
//		Value value) const noexcept
//{
//	auto sen = sensor_type_.find(std::make_tuple(id, type, type));
//	if(sen == sensor_type_.end())
//	{
//		return false;
//	}
//
//	bool flag = false;
//	for(auto const& s : sen->second)
//	{
//		if(!s.enabled)
//		{
//			continue;
//		}
//		if(compare(s.stype, s.value, static_cast<float>(value)))
//		{
//			if(!s.status)
//			{
//				flag = true;
//				s.status = true;
//			}
//		}
//		else
//		{
//			s.status = false;
//		}
//	}
//
//	return flag;
//}

template<typename Value>
std::vector<sensor_notify> Notify::can(Device::device_id id,
		unsigned type, unsigned index,
		Value value) const noexcept
{
	auto sen = sensor_type_.find(std::make_tuple(id, type, type));
	if(sen == sensor_type_.end())
	{
		return false;
	}

	std::vector<sensor_notify> vsn;
	for(auto const& s : sen->second)
	{
		if(!s.enabled)
		{
			continue;
		}
		if(compare(s.stype, s.value, static_cast<float>(value)))
		{
			if(!s.status)
			{
				s.status = true;
				vsn.emplace_back(s);
			}
		}
		else
		{
			s.status = false;
		}
	}

	return vsn;
}

}//Notify
}//Agro

#endif /* AGRO_DAEMON_NOTIFY_NOTIFY_TYPE_IMPL_HPP__ */
