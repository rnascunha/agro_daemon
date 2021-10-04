#include "list.hpp"
//#include "../notify/notify.hpp"

namespace Agro{
namespace Device{

Device_List::Device_List(){}

std::map<mesh_addr_t const, Device> const& Device_List::list() const noexcept
{
	return list_;
}

Device* Device_List::operator[](mesh_addr_t const& addr) noexcept
{
	auto dev = list_.find(addr);
	if(dev == list_.end())
	{
		return nullptr;
	}

	return &dev->second;
}

Device const* Device_List::operator[](mesh_addr_t const& addr) const noexcept
{
	auto dev = list_.find(addr);
	if(dev == list_.end())
	{
		return nullptr;
	}

	return &dev->second;
}

Device* Device_List::operator[](const char* addr_str) noexcept
{
	std::error_code ec;
	mesh_addr_t addr{addr_str, static_cast<unsigned>(std::strlen(addr_str)), ec};
	if(ec) return nullptr;

	auto dev = list_.find(addr);
	if(dev == list_.end())
	{
		return nullptr;
	}

	return &dev->second;
}

Device const* Device_List::operator[](const char* addr_str) const noexcept
{
	std::error_code ec;
	mesh_addr_t addr{addr_str, static_cast<unsigned>(std::strlen(addr_str)), ec};
	if(ec) return nullptr;

	auto dev = list_.find(addr);
	if(dev == list_.end())
	{
		return nullptr;
	}

	return &dev->second;
}

Device* Device_List::add(Device&& device) noexcept
{
	auto dev = list_.find(device.mac());
	if(dev != list_.end())
	{
		return nullptr;
	}

	return &list_.emplace(device.mac(), device).first->second;
}

Device& Device_List::add_or_get(mesh_addr_t const& mac) noexcept
{
	auto dev = list_.find(mac);
	if(dev != list_.end())
	{
		return dev->second;
	}

	return list_.emplace(mac, Device{mac}).first->second;
}

}//Device
}//Agro
