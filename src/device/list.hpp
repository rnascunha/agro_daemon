#ifndef AGRO_DAEMON_DEVICE_LIST_HPP__
#define AGRO_DAEMON_DEVICE_LIST_HPP__

#include <map>

#include "types.hpp"
#include "../resources/types.hpp"
#include "device.hpp"

class Device_List{
	public:
		Device_List();

		/**
		 * Updates
		 */
		Device& update(mesh_addr_t const&, endpoint const&, Resource::status const&) noexcept;
		Device& update(mesh_addr_t const&, endpoint const&, Resource::config const&) noexcept;
		Device& update(mesh_addr_t const&, endpoint const&, Resource::route const&,
				const uint8_t* children, std::size_t children_size) noexcept;
		Device& update(mesh_addr_t const&, endpoint const&, Resource::full_config const&,
				const uint8_t* children, std::size_t children_size) noexcept;
		Device& update(mesh_addr_t const&, endpoint const&, Resource::board_config const&,
				const char* version, std::size_t version_len) noexcept;
	private:
		std::map<mesh_addr_t, Device> list_;
};

#endif /* AGRO_DAEMON_DEVICE_LIST_HPP__ */
