#ifndef AGRO_DAEMON_DEVICE_HPP__
#define AGRO_DAEMON_DEVICE_HPP__

#include <cstdint>
#include <string>
#include <vector>

#include "types.hpp"
#include "../resources/types.hpp"
#include "../error.hpp"
#include "../coap_engine.hpp"

class Device{
	public:
		Device();			//Must be default constructible to use map
		Device(mesh_addr_t const& mac_);
		Device(const char* mac_str, unsigned, Error& ec);

		/**
		 * Getters
		 */
		mesh_addr_t const& mac() const noexcept;
		mesh_addr_t const & parent() const noexcept;
		mesh_addr_t const& mac_ap() const noexcept;
		mesh_addr_t const& net_id() const noexcept;

		endpoint const& get_endpoint() const noexcept;

		std::string name() const noexcept;

		std::uint8_t channel_config() const noexcept;
		std::uint8_t channel() const noexcept;

		std::uint8_t layer() const noexcept;

		std::vector<mesh_addr_t> const& children_table() const noexcept;
		std::vector<mesh_addr_t> const& children() const noexcept;

		std::string firmware_version() const noexcept;
		std::string hardware_version() const noexcept;

		Value_List<std::int8_t> const& rssi() const noexcept;

		bool has_rtc() const noexcept;
		bool has_temperature_sensor() const noexcept;

		/**
		 * Updates
		 */
		void update(endpoint const&, Resource::status const&) noexcept;
		void update(endpoint const&, Resource::config const&) noexcept;
		void update(endpoint const&, Resource::route const&,
				const uint8_t* children, std::size_t children_size) noexcept;
		void update(endpoint const&, Resource::full_config const&,
				const uint8_t* children, std::size_t children_size) noexcept;
		void update(endpoint const&, Resource::board_config const&,
				const char* version, std::size_t version_len) noexcept;
	private:
		void update_endpoint(endpoint const& ep) noexcept;

		mesh_addr_t	mac_;
		mesh_addr_t parent_;
		mesh_addr_t mac_ap_;
		mesh_addr_t net_id_;

		endpoint	ep_;

		std::string name_ = {};

		std::uint8_t ch_config_ = 0;
		std::uint8_t ch_conn_ = 0;

		std::uint8_t layer_ = 0;
		std::vector<mesh_addr_t> children_table_;
		std::vector<mesh_addr_t> children_;

		Value_List<std::int8_t> rssi_;

		//Agro values
		std::string fw_version_ = "";
		std::string hw_version_ = "";

		bool has_rtc_ = false;
		bool has_sensor_temp_ = false;
};

#endif /* AGRO_DAEMON_DEVICE_HPP__ */
