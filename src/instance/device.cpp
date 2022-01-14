#include "agro.hpp"
#include "tt/tt.hpp"

namespace Agro{

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

	*dev = device_list_[host];
	if(!(*dev))
	{
		//Device does's exists
		Device::device_id id;
		db_.add_device(host, id);
		*dev = device_list_.add(Device::Device{id, host});
		tt::status("New device connected [%s]", host.to_string().c_str());

		db_.set_device_state(**dev, true);

		std::string msg = "New device added: ";
		msg += host.to_string();
		notify_all_policy(Authorization::rule::view_device, msg);
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
