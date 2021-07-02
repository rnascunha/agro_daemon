#include "list.hpp"

Device_List::Device_List(){}

std::map<mesh_addr_t, Device> const& Device_List::list() const noexcept
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

Device& Device_List::update_name(mesh_addr_t const& addr, std::string const& name) noexcept
{
	auto dev = list_.find(addr);
	if(dev != list_.end())
	{
		dev->second.name(name);
		return dev->second;
	}

	auto resp = list_.emplace(addr, Device{addr}).first;
	resp->second.name(name);

	return resp->second;
}

Device& Device_List::update_ac_load(mesh_addr_t const& addr, unsigned index, bool value) noexcept
{
	auto dev = list_.find(addr);
	if(dev != list_.end())
	{
		dev->second.update_ac_load(index, value);
		return dev->second;
	}

	auto resp = list_.emplace(addr, Device{addr}).first;
	resp->second.update_ac_load(index, value);

	return resp->second;
}

Device& Device_List::update(mesh_addr_t const& addr,
		endpoint const& ep,
		Resource::status const& sts) noexcept
{
	auto dev = list_.find(addr);
	if(dev != list_.end())
	{
		dev->second.update(ep, sts);
		return dev->second;
	}

	auto resp = list_.emplace(addr, Device{addr}).first;
	resp->second.update(ep, sts);

	return resp->second;
}

Device& Device_List::update(mesh_addr_t const& addr,
		endpoint const& ep,
		Resource::config const& cfg) noexcept
{
	auto dev = list_.find(addr);
	if(dev != list_.end())
	{
		dev->second.update(ep, cfg);
		return dev->second;;
	}

	auto resp = list_.emplace(addr, Device{addr}).first;
	resp->second.update(ep, cfg);

	return resp->second;
}

Device& Device_List::update(mesh_addr_t const& addr,
		endpoint const& ep, Resource::route const& rt,
		const uint8_t* children, std::size_t children_size) noexcept
{
	auto dev = list_.find(addr);
	if(dev != list_.end())
	{
		dev->second.update(ep, rt, children, children_size);
		return dev->second;;
	}

	auto resp = list_.emplace(addr, Device{addr}).first;
	resp->second.update(ep, rt, children, children_size);

	return resp->second;
}

Device& Device_List::update(mesh_addr_t const& addr,
		endpoint const& ep, Resource::full_config const& cfg,
		const uint8_t* children, std::size_t children_size) noexcept
{
	auto dev = list_.find(addr);
	if(dev != list_.end())
	{
		dev->second.update(ep, cfg, children, children_size);
		return dev->second;;
	}

	auto resp = list_.emplace(addr, Device{addr}).first;
	resp->second.update(ep, cfg, children, children_size);

	return resp->second;
}

Device& Device_List::update(mesh_addr_t const& addr,
		endpoint const& ep, Resource::board_config const& cfg,
		const char* version, std::size_t version_len) noexcept
{
	auto dev = list_.find(addr);
	if(dev != list_.end())
	{
		dev->second.update(ep, cfg, version, version_len);
		return dev->second;
	}

	auto resp = list_.emplace(addr, Device{addr}).first;
	resp->second.update(ep, cfg, version, version_len);

	return resp->second;
}

Device& Device_List::update(mesh_addr_t const& addr,
		endpoint const& ep,
		Resource::sensor_data const& sen) noexcept
{
	auto dev = list_.find(addr);
	if(dev != list_.end())
	{
		dev->second.update(ep, sen);
		return dev->second;;
	}

	auto resp = list_.emplace(addr, Device{addr}).first;
	resp->second.update(ep, sen);

	return resp->second;
}
