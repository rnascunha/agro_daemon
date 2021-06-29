#include "list.hpp"

Device_List::Device_List(){}

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
