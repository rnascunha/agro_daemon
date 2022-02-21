#ifndef AGRO_DAEMON_DEVICE_HPP__
#define AGRO_DAEMON_DEVICE_HPP__

#include <cstdint>
#include <string>
#include <vector>

#include "types.hpp"
#include "resources/types.hpp"
#include "../error.hpp"
#include "../coap_engine.hpp"
#include "../sensor/value.hpp"
#include "../sensor/sensor_list.hpp"
#include "../sensor/sensor_type.hpp"

namespace Agro{
namespace Device{

using device_id = int;

class Net;

class Device{
	public:
		Device();			//Must be default constructible to use map
		Device(mesh_addr_t const& mac_);
		Device(const char* mac_str, unsigned, Error& ec);
		Device(device_id, mesh_addr_t const&);
		Device(device_id, mesh_addr_t const&,
				mesh_addr_t const& mac_ap,
				mesh_addr_t const& parent,
				endpoint const& ep,
				std::string const& name = "",
				std::string const& fw_version = "",
				std::string const& hw_version = "",
				uint8_t channel_config = 0, uint8_t channel = 0,
				bool has_rtc = false, bool has_temp_sensor = false,
				int layer = -1);

		device_id id() const noexcept;

		//Net
		Net const* net() const noexcept;
		void net(Net const*) noexcept;
		mesh_addr_t const& net_id() const noexcept;
		void net_id(mesh_addr_t const&) noexcept;

		//mac
		mesh_addr_t const& mac() const noexcept;

		//parent
		mesh_addr_t const& parent() const noexcept;
		void parent(mesh_addr_t const&) noexcept;

		//mac_ap
		mesh_addr_t const& mac_ap() const noexcept;
		void mac_ap(mesh_addr_t const&) noexcept;

		//endpoint
		endpoint const& get_endpoint() const noexcept;
		void update_endpoint(endpoint const& ep) noexcept;

		//Name
		std::string name() const noexcept;
		void name(std::string const& name) noexcept;

		//Channel
		std::uint8_t channel_config() const noexcept;
		std::uint8_t channel() const noexcept;
		void channel(uint8_t ch_cfg, uint8_t ch) noexcept;

		//Layer
		int layer() const noexcept;
		void layer(int) noexcept;

		//Children
		std::vector<mesh_addr_t> const& children_table() const noexcept;
		void clear_children() noexcept;
		void add_children(mesh_addr_t const&) noexcept;

		//Firmare
		std::string const& firmware_version() const noexcept;
		std::string const& hardware_version() const noexcept;
		void version(std::string const& fw, std::string const hw) noexcept;

		//config
		bool has_rtc() const noexcept;
		bool has_temperature_sensor() const noexcept;
		void has_config(bool rtc, bool temp_sensor) noexcept;

		//last packet time
		std::uint32_t last_packet_time() const noexcept;
		void update_last_packet_time() noexcept;

		//rtc
		Sensor::Value<value_time> const& rtc_time() const noexcept;
		void update_rtc_time(value_time) noexcept;

		//uptime
		Sensor::Value<int64_t> const& uptime() const noexcept;
		void update_uptime(int64_t) noexcept;

		//fuse
		std::int32_t fuse() const noexcept;
		void fuse(std::int32_t) noexcept;

		//Sensor
		Sensor::Sensor_List const& sensor_list() const noexcept;
		bool update_sensor(unsigned type, unsigned index,
						value_time time, Sensor::sensor_value const&,
						bool add_change = false) noexcept;

		int update_ac_load(unsigned index, bool value) noexcept;

		//Apps
		void add_app(std::string const&, std::size_t, const sha256_hash) noexcept;
		void clear_apps() noexcept;
		void delete_app(std::string const&) noexcept;
		std::vector<app> const& apps() const noexcept;

		//Jobs
		void jobs(std::uint8_t const* jobs, std::size_t size) noexcept;
		std::vector<job> const& jobs() const noexcept;

		bool operator==(Device const& rhs) const noexcept;
	private:
		device_id	id_ = -1;

		mesh_addr_t	mac_;
		mesh_addr_t parent_;
		mesh_addr_t mac_ap_;
		mesh_addr_t net_id_;
		Net const*	net_ = nullptr;

		endpoint	ep_{};

		std::string name_{};

		std::uint8_t ch_config_{0};
		std::uint8_t ch_conn_{0};

		int layer_{-2};
		std::vector<mesh_addr_t> children_table_;

		//Agro values
		std::string fw_version_{};
		std::string hw_version_{};

		std::uint32_t	last_packet_time_{0};

		bool has_rtc_{false};
		bool has_sensor_temp_{false};

		Sensor::Sensor_List slist_;

		Sensor::Value<int64_t>		uptime_;
		Sensor::Value<value_time>	rtc_time_;
		std::int32_t 		fuse_{0};

		std::vector<job>	jobs_;
		std::vector<app>	apps_;
};

}//Device
}//Agro


#endif /* AGRO_DAEMON_DEVICE_HPP__ */
