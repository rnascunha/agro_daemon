#ifndef AGRO_MESH_DEVICE_TYPES_HPP__
#define AGRO_MESH_DEVICE_TYPES_HPP__

#include <cstdint>
#include <vector>
#include <system_error>

#include "../helper/sha256.hpp"
#include "resources/types.hpp"
#include "../error.hpp"

static constexpr const std::size_t max_app_name_size = 12;

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

std::ostream& operator<< (std::ostream& stream, const mesh_addr_t& addr) noexcept;

using value_time = std::uint32_t;

value_time get_time() noexcept;

enum class dow : std::uint8_t{
	monday = 1 << 0,
	tuesday = 1 << 1,
	wednesday = 1 << 2,
	thursday = 1 << 3,
	friday =  1 << 4,
	saturday = 1 << 5,
	sunday = 1 << 6
};

struct __attribute__((packed)) job_packet{
	std::uint8_t be_time_hour;
	std::uint8_t be_time_minute;
	std::uint8_t af_time_hour;
	std::uint8_t af_time_minute;
	dow			 day_of_week;
	std::uint8_t priority;
};

struct __attribute__((packed)) job_packet_executor{
	std::int32_t	arg;
	char			exec[max_app_name_size];
};

struct job{
	static constexpr const std::size_t packet_size = sizeof(job_packet) + sizeof(job_packet_executor);

	struct time{
		std::uint8_t	hour = 0;
		std::uint8_t	minute = 0;
	};

	job(job_packet const* data, job_packet_executor const* data_exec)
	 : time_before{data->be_time_hour, data->be_time_minute},
	   time_after{data->af_time_hour, data->af_time_minute},
	   day_of_week{data->day_of_week},
	   priority{data->priority},
	   app_name{data_exec->exec},
	   argument{data_exec->arg}
	   {}

	time			time_before;
	time			time_after;
	dow				day_of_week;
	std::uint8_t	priority;

	std::string		app_name;
	std::int32_t	argument;
};

struct app
{
	app(std::string const& app_name, std::size_t app_size, const sha256_hash hash);

	std::string name;
	std::size_t size;
	sha256_hash	hash;
};

#endif /* AGRO_MESH_DEVICE_TYPES_HPP__ */
