#include "agro.hpp"
#include "tt/tt.hpp"
#include "coap-te-debug.hpp"
#include "../device/resources/process.hpp"
#include "../user/policy.hpp"
#include "../helper/time_helper.hpp"

static constexpr const int interval_check = 45; //seconds
static const std::uint32_t time_invalidate_node = 60000; //milliseconds

namespace Agro{

void instance::initiate_check_roots() noexcept
{
	check_root_timer_.expires_after(std::chrono::seconds(interval_check));
	check_root_timer_.async_wait([this](const boost::system::error_code&){
		check_network_roots();
		initiate_check_roots();
	});
}

bool instance::check_root(mesh_addr_t const& addr) noexcept
{
	auto* dev = device_list_[addr];
	if(!dev)
	{
		tt::error("['check_root'] Device not found!");
		return false;
	}

	if((static_cast<std::uint32_t>(time_epoch_miliseconds()) - dev->last_packet_time()) > time_invalidate_node
		&& dev->layer() == 1)
	{
		tt::warning("['check_root'] Removing root device! [%s]", addr.to_string().c_str());
		return false;
	}
	return true;
}

void instance::check_network_roots() noexcept
{
	auto& tree = tree_.get_endpoint();
	std::vector<mesh_addr_t> to_remove;
	for(auto& [addr, ep] : tree)
	{
		if(!check_root(addr))
		{
			to_remove.push_back(addr);
		}
	}

	for(auto const& r : to_remove)
	{
		remove_node_from_tree(r);
	}
}

}//Agro
