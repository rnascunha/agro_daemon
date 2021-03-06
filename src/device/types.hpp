#ifndef AGRO_MESH_DEVICE_TYPES_HPP__
#define AGRO_MESH_DEVICE_TYPES_HPP__

#include <cstdint>
#include <vector>
#include <system_error>

#include "resources/types.hpp"
#include "../error.hpp"

struct mesh_addr_t{
	static constexpr const unsigned str_size = 18;

	std::uint8_t addr[6];    /**< mac address */

	mesh_addr_t();
	mesh_addr_t(mesh_addr_t const&);
	mesh_addr_t(std::uint8_t const*);
	mesh_addr_t(const char*, unsigned, std::error_code&);

    bool set(const char*, unsigned) noexcept;

    bool operator==(mesh_addr_t const& rhs) const noexcept;
	bool operator!=(mesh_addr_t const& rhs) const noexcept;
	/**
	 * Needed to be used as a key to a map
	 */
	bool operator<(mesh_addr_t const& rhs) const noexcept;

	mesh_addr_t& operator=(mesh_addr_t const&) noexcept;
	mesh_addr_t& operator=(Agro::Device::Resource::mesh_addr_t const&) noexcept;

	char* to_string(char*, unsigned = 18) const noexcept;
	std::string to_string() const noexcept;
};

using value_time = std::uint32_t;

value_time get_time() noexcept;

struct job{
	static constexpr const std::size_t packet_size = 7;
	struct time{
		std::uint8_t	hour = 0;
		std::uint8_t	minute = 0;
	};

	job(std::uint8_t const* data)
	 : time_before{data[0], data[1]},
	   time_after{data[2], data[3]},
	   dow{data[4]},
	   priority{data[5]},
	   active{data[6]}
	   {}

	time			time_before;
	time			time_after;
	std::uint8_t	dow;
	std::uint8_t	priority;
	std::uint8_t	active;
};

struct app{
	app(std::string const& app_name, std::size_t app_size)
		: name(app_name), size(app_size){}

	std::string name;
	std::size_t size;
};

#endif /* AGRO_MESH_DEVICE_TYPES_HPP__ */
