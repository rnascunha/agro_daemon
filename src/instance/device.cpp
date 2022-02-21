#include "agro.hpp"
#include "tt/tt.hpp"
#include <sstream>

namespace Agro{

std::pair<Device::Device*, bool>
		instance::add_or_get_device(mesh_addr_t const& mac) noexcept
{
	std::pair<Device::Device*, bool> ret{nullptr, false};
	ret.first = device_list_[mac];
	if(! (ret.first))
	{
		//Device does's exists
		ret.second = true;
		Device::device_id id;
		db_.add_device(mac, id);
		ret.first = device_list_.add(Device::Device{id, mac});
	}

	return ret;
}

void instance::device_connected(std::vector<Device::Device const*> const& list) noexcept
{
	if(!list.size()) return;

	std::stringstream ss;
	ss << "New device added: ";

	bool flag = false;
	for(auto const* device : list)
	{
		tt::status("New device connected [%s]", device->mac().to_string().c_str());
		db_.set_device_state(*device, true);

		if(flag) ss << ", ";
		else flag = true;

		ss << device->mac();
	}
	notify_all_policy(Authorization::rule::view_device, ss.str());
}

bool instance::process_device_request(engine::message const& request,
		Device::Device** dev,
		CoAP::Message::Option::option& op) noexcept
{
	*dev = nullptr;
	if(!CoAP::Message::Option::get_option(request,
			op,
			CoAP::Message::Option::code::uri_host))
	{
		tt::debug("Ignoring device packet! Missing uri-host option");
		return false;
	}

	std::error_code ec;
	mesh_addr_t host{static_cast<const char*>(op.value), op.length, ec};
	if(ec)
	{
		tt::debug("Ignoring device packet! Error parsing address [%d/%s]", ec.value(), ec.message().c_str());
		return false;
	}

	auto ret = add_or_get_device(host);
	*dev = ret.first;
	if(ret.second)
	{
		device_connected({*dev});
	}

	return true;
}

bool instance::update_db_device(Device::Device const& device) noexcept
{
	int rc = db_.update_device(device);
	if(rc != SQLITE_DONE)
	{
		tt::error("Error updating device database [%d]", rc);
	}
	return rc == SQLITE_DONE;
}

Device::Net* instance::get_or_add_net(mesh_addr_t const& net_addr) noexcept
{
	Device::Net* net = net_list_.get(net_addr);
	if(!net)
	{
		int id;
		db_.add_net(net_addr, id);
		net = net_list_.add(Device::Net{id, net_addr});
	}

	return net;
}


}//Agro
