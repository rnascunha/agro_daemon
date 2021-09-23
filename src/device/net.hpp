#ifndef AGRO_DAEMON_DEVICE_NET_HPP__
#define AGRO_DAEMON_DEVICE_NET_HPP__

#include "types.hpp"
#include <string>
#include <unordered_map>

namespace Agro{
namespace Device{

using net_id = int;

class Net{
	public:
		Net(net_id, mesh_addr_t const&, std::string const& = "");

		net_id id() const noexcept;
		mesh_addr_t const& net_addr() const noexcept;
		std::string const& name() const noexcept;
	private:
		net_id		id_ = -1;
		mesh_addr_t	net_addr_;
		std::string name_;
};

class Net_List{
	public:
		Net_List();

		Net* add(Net&&) noexcept;

		Net const* get(net_id) const noexcept;
		Net* get(net_id) noexcept;

		Net const* get(mesh_addr_t const&) const noexcept;
		Net* get(mesh_addr_t const&) noexcept;
	private:
		std::unordered_map<net_id, Net> list_;
};

}//Device
}//Agro

#endif /* AGRO_DAEMON_DEVICE_NET_HPP__ */
