#ifndef AGRO_DAEMON_DEVICE_HPP__
#define AGRO_DAEMON_DEVICE_HPP__

#include <cstdint>
#include <string>
#include <vector>

#include "types.hpp"
#include "../resources/types.hpp"
#include "../error.hpp"
#include "../coap_engine.hpp"

namespace Agro{
namespace Device{

using device_id = int;

class Net;

class Device{
	public:
		Device();			//Must be default constructible to use map
		Device(mesh_addr_t const& mac_);
		Device(const char* mac_str, unsigned, Error& ec);
		Device(device_id, mesh_addr_t const&,
				std::string const& name = "",
				std::string const& fw_version = "",
				std::string const& hw_version = "");

		/**
		 * Getters
		 */
		device_id id() const noexcept;
		Net const* net() const noexcept;
		void net(Net const*) noexcept;

		mesh_addr_t const& mac() const noexcept;
		mesh_addr_t const& parent() const noexcept;
		mesh_addr_t const& mac_ap() const noexcept;
		mesh_addr_t const& net_id() const noexcept;

		endpoint const& get_endpoint() const noexcept;

		std::string name() const noexcept;
		void name(std::string const& name) noexcept;

		std::uint8_t channel_config() const noexcept;
		std::uint8_t channel() const noexcept;

		int layer() const noexcept;

		std::vector<mesh_addr_t> const& children_table() const noexcept;
		std::vector<mesh_addr_t> const& children() const noexcept;

		std::string const& firmware_version() const noexcept;
		std::string const& hardware_version() const noexcept;

		Value_List<std::int8_t> const& rssi() const noexcept;

		bool has_rtc() const noexcept;
		bool has_temperature_sensor() const noexcept;

		std::uint32_t last_packet_time() const noexcept;
		Value_List<float> const& temperature() const noexcept;
		Value_List<std::uint8_t> const& gpios() const noexcept;

		Value<value_time> const& rtc_time() const noexcept;
		Value<int64_t> const& uptime() const noexcept;

		std::int32_t fuse() const noexcept;
		void fuse(std::int32_t) noexcept;

		void jobs(std::uint8_t const* jobs, std::size_t size) noexcept;
		std::vector<job> const& jobs() const noexcept;
		std::vector<app> const& apps() const noexcept;

		/**
		 * Updates
		 */
		void clear_children() noexcept;
		void add_children(mesh_addr_t const&) noexcept;

		void update(endpoint const&, Resource::status const&) noexcept;
		void update(endpoint const&, Resource::config const&, Net* net) noexcept;
		void update(endpoint const&, Resource::route const&,
				const uint8_t* children, std::size_t children_size) noexcept;
		void update(endpoint const&, Resource::full_config const&,
				const uint8_t* children, std::size_t children_size) noexcept;
		void update(endpoint const&, Resource::board_config const&,
				const char* version, std::size_t version_len) noexcept;
		void update(endpoint const&, Resource::sensor_data const&) noexcept;

		bool update_ac_load(unsigned index, bool value) noexcept;
		void update_uptime(int64_t) noexcept;
		void update_rtc_time(value_time) noexcept;
		void add_app(std::string const&, std::size_t) noexcept;
		void delete_app(std::string const&) noexcept;
	private:
		void update_endpoint(endpoint const& ep) noexcept;

		device_id	id_ = -1;

		mesh_addr_t	mac_;
		mesh_addr_t parent_;
		mesh_addr_t mac_ap_;
		mesh_addr_t net_id_;
		Net const*	net_ = nullptr;

		endpoint	ep_;

		std::string name_ = {};

		std::uint8_t ch_config_ = 0;
		std::uint8_t ch_conn_ = 0;

		int layer_ = -1;
		std::vector<mesh_addr_t> children_table_;
		std::vector<mesh_addr_t> children_;

		Value_List<std::int8_t> rssi_;

		//Agro values
		std::string fw_version_ = "";
		std::string hw_version_ = "";

		std::uint32_t	last_packet_time_ = 0;

		bool has_rtc_ = false;
		bool has_sensor_temp_ = false;

		Value_List<float> temp_;
		Value_List<std::uint8_t> gpios_;

		Value<int64_t>		uptime_;
		Value<value_time>	rtc_time_;
		std::int32_t 		fuse_ = 0;

		std::vector<job>	jobs_;
		std::vector<app>	apps_;
};

}//Device
}//Agro


#endif /* AGRO_DAEMON_DEVICE_HPP__ */
