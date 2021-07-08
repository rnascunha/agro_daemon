#ifndef AGRO_DAEMON_DEVICE_LIST_HPP__
#define AGRO_DAEMON_DEVICE_LIST_HPP__

#include <map>

#include "types.hpp"
#include "../resources/types.hpp"
#include "device.hpp"

class Device_List{
	public:
		Device_List();

		std::map<mesh_addr_t, Device> const& list() const noexcept;

		Device* operator[](mesh_addr_t const&) noexcept;
		Device const* operator[](mesh_addr_t const&) const noexcept;

		Device* operator[](const char*) noexcept;
		Device const* operator[](const char*) const noexcept;

		/**
		 * Updates
		 */
		Device& update_name(mesh_addr_t const&, std::string const&) noexcept;
		Device& update_ac_load(mesh_addr_t const&, unsigned index, bool value) noexcept;
		Device& update_rtc_time(mesh_addr_t const&, value_time) noexcept;
		Device& update_uptime(mesh_addr_t const&, int64_t) noexcept;

		Device& update(mesh_addr_t const&, endpoint const&, Resource::status const&) noexcept;
		Device& update(mesh_addr_t const&, endpoint const&, Resource::config const&) noexcept;
		Device& update(mesh_addr_t const&, endpoint const&, Resource::route const&,
				const uint8_t* children, std::size_t children_size) noexcept;
		Device& update(mesh_addr_t const&, endpoint const&, Resource::full_config const&,
				const uint8_t* children, std::size_t children_size) noexcept;
		Device& update(mesh_addr_t const&, endpoint const&, Resource::board_config const&,
				const char* version, std::size_t version_len) noexcept;
		Device& update(mesh_addr_t const&, endpoint const&, Resource::sensor_data const&) noexcept;
	private:
		std::map<mesh_addr_t, Device> list_;
};

#endif /* AGRO_DAEMON_DEVICE_LIST_HPP__ */
