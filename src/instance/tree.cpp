#include "agro.hpp"
#include <algorithm>

namespace Agro{

bool instance::remove_node_from_tree(mesh_addr_t const& addr) noexcept
{
	bool change = tree_.remove_node(addr);
	if(change)
	{
		check_device_state();
		share_->write_all_policy(Authorization::rule::view_device,
				std::make_shared<std::string>(Device::Message::device_tree_to_json(tree_)));
	}
	return change;
}

bool instance::update_tree(Device::Device const& device) noexcept
{
	bool change = tree_.update(device);
	if(change)
	{
		check_device_state();
		share_->write_all_policy(Authorization::rule::view_device,
				std::make_shared<std::string>(Device::Message::device_tree_to_json(tree_)));
	}
	return change;
}

void instance::check_device_state() noexcept
{
	std::vector<mesh_addr_t> new_unconnected = tree_.unconnected();
	std::vector<std::reference_wrapper<Device::Device const>>	new_connected,
									new_disconnected;
	for(auto const& [addr, device] : device_list_.list())
	{
		bool has_new = std::find(new_unconnected.begin(), new_unconnected.end(), addr) != new_unconnected.end(),
			has_old = std::find(unconnected_.begin(), unconnected_.end(), addr) != unconnected_.end();

		if(has_new != has_old)
		{
			db_.set_device_state(device, has_old);
			tt::status("Device [%s] %s", addr.to_string().c_str(), has_old ? "connected" : "disconnected");

			if(has_old) new_connected.push_back(std::cref(device));
			else new_disconnected.push_back(std::cref(device));
		}
	}

	if(!new_connected.empty())
	{
		notify_all_policy(Authorization::rule::view_device,
				Notify::device_type::connect,
				new_connected,
				"connected");
//		notify_all_policy(
//				Authorization::rule::view_device,
//				Notify::general_type::device_connect,
//				Notify::Message::make_status_devices(new_connected, "connected"));
	}

	if(!new_disconnected.empty())
	{
		notify_all_policy(Authorization::rule::view_device,
						Notify::device_type::disconnect,
						new_disconnected,
						"disconnected");
//		notify_all_policy(
//					Authorization::rule::view_device,
//					Notify::general_type::device_disconnect,
//					Notify::Message::make_status_devices(new_disconnected, "disconnected"));
	}

	unconnected_ = std::move(new_unconnected);
}

}//Agro
