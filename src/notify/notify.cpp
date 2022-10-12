#include "notify.hpp"
#include "../helper/enum_ops.hpp"

namespace Agro{
namespace Notify{

sensor_notify::sensor_notify(
		bool enabled_,
		sensor_type stype_,
		float value_,
		bool status_)
	: enabled(enabled_), stype(stype_),
	  value(value_), status(status_){}

sensor_temp_notify::sensor_temp_notify(
		unsigned type_,
		unsigned index_,
		bool enabled_,
		sensor_type stype_,
		float value_,
		bool status_)
	: type(type_), index(index_),
	  enabled(enabled_), stype(stype_),
	  value(value_), status(status_){}

Notify::Notify(){}

void Notify::set(general_type type) noexcept
{
	type_ = type;
}

general_type Notify::get() const noexcept
{
	return type_;
}

bool Notify::can(general_type type) const noexcept
{
	return type & type_;
}

void Notify::set(Device::device_id id, device_type type) noexcept
{
	auto const dev = dev_type_.find(id);
	if(dev == dev_type_.end())
	{
		dev_type_.emplace(id, type);
		return;
	}

	dev->second = type;
}

Notify::device_container const&
Notify::get_devices() const noexcept
{
	return dev_type_;
}

bool Notify::can(Device::device_id id, device_type noti) const noexcept
{
	auto const dev = dev_type_.find(id);
	if(dev == dev_type_.end())
	{
		return false;
	}

	return dev->second & noti;
}

void Notify::set(Device::device_id id,
				std::vector<sensor_temp_notify> const& values) noexcept
{
	//Erasing old device notify definition
	for (auto it = sensor_type_.cbegin(); it != sensor_type_.cend() /* not hoisted */; /* no increment */)
	{
		if (std::get<0>(it->first) == id)
		{
			sensor_type_.erase(it++);
		}
		else
		{
			++it;
		}
	}

	//Inserting new defitions
	for(auto const& s : values)
	{
		auto const key = std::make_tuple(id, s.type, s.index);
		auto const it = sensor_type_.find(key);
		if(it == sensor_type_.end())
		{
			sensor_type_.emplace(key, std::vector<sensor_notify>{sensor_notify{s.enabled, s.stype, s.value, false}});
			continue;
		}
		it->second.emplace_back(s.enabled, s.stype, s.value, false);
	}
}

void Notify::add(Device::device_id id, unsigned type, unsigned index,
				bool enabled, sensor_type stype, float value, bool status /* = false */) noexcept
{
	auto const key = std::make_tuple(id, type, index);
	auto const it = sensor_type_.find(key);
	if(it == sensor_type_.end())
	{
		sensor_type_.emplace(key, std::vector<sensor_notify>{sensor_notify{enabled, stype, value, status}});
		return;
	}
	it->second.emplace_back(enabled, stype, value, status);
}

Notify::sensor_container const& Notify::get_sensors() const noexcept
{
	return sensor_type_;
}

Notify::sensor_container_value const* Notify::get_device_sensors(
				Device::device_id id, unsigned type, unsigned) const noexcept
{
	auto sen = sensor_type_.find(std::make_tuple(id, type, type));
	if(sen == sensor_type_.end())
	{
		return nullptr;
	}
	return &sen->second;
}

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

std::vector<sensor_notify> Notify::can(Device::device_id id,
		unsigned type, unsigned index,
		float value) noexcept
{
	std::vector<sensor_notify> vsn;

	auto sen = sensor_type_.find(std::make_tuple(id, type, index));
	if(sen == sensor_type_.end())
	{
		return vsn;
	}

	for(auto& s : sen->second)
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
