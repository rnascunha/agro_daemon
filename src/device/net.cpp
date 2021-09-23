#include "net.hpp"
#include <algorithm>

namespace Agro{
namespace Device{

Net::Net(net_id id, mesh_addr_t const& number, std::string const& name /* = "" */)
	: id_(id), net_addr_(number), name_(name){}

net_id Net::id() const noexcept
{
	return id_;
}

mesh_addr_t const& Net::net_addr() const noexcept
{
	return net_addr_;
}

std::string const& Net::name() const noexcept
{
	return name_;
}

Net_List::Net_List(){}

Net* Net_List::add(Net&& net) noexcept
{
	auto n = list_.find(net.id());
	if(n != list_.end())
	{
		return nullptr;
	}

	return &list_.emplace(net.id(), net).first->second;
}

Net const* Net_List::get(net_id id) const noexcept
{
	auto n = list_.find(id);
	if(n == list_.end())
	{
		return nullptr;
	}
	return &n->second;
}

Net* Net_List::get(net_id id) noexcept
{
	auto n = list_.find(id);
	if(n == list_.end())
	{
		return nullptr;
	}
	return &n->second;
}

Net const* Net_List::get(mesh_addr_t const& number) const noexcept
{
	for(auto const& n : list_)
	{
		if(n.second.net_addr() == number)
		{
			return &n.second;
		}
	}
	return nullptr;
}

Net* Net_List::get(mesh_addr_t const& number) noexcept
{
	for(auto& n : list_)
	{
		if(n.second.net_addr() == number)
		{
			return &n.second;
		}
	}
	return nullptr;
}

}//Device
}//Agro
