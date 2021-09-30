#ifndef AGRO_DAEMON_DEVICE_LIST_HPP__
#define AGRO_DAEMON_DEVICE_LIST_HPP__

#include <map>

#include "types.hpp"
#include "../resources/types.hpp"
#include "device.hpp"

namespace Agro{
namespace Device{

class Device_List{
	public:
		Device_List();

		std::map<mesh_addr_t const, Device> const& list() const noexcept;

		Device* operator[](mesh_addr_t const&) noexcept;
		Device const* operator[](mesh_addr_t const&) const noexcept;

		Device* operator[](const char*) noexcept;
		Device const* operator[](const char*) const noexcept;

		Device* add(Device&&) noexcept;
		Device* add_or_get(mesh_addr_t const&) noexcept;
	private:
		std::map<mesh_addr_t const, Device> list_;
};

}//Device
}//Agro

#endif /* AGRO_DAEMON_DEVICE_LIST_HPP__ */
