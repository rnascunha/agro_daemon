#include "device.hpp"
#include "../helper/time_helper.hpp"

#define GPIOS_ARRAY_SENSOR_TYPE		5
#define GPIOS_ARRAY_SENSOR_INDEX	0

namespace Agro{
namespace Device{

Device::Device(){}

Device::Device(mesh_addr_t const& mac)
	: mac_(mac)
{
	char buf[18];
	name_ = mac.to_string(buf, 18);
}

Device::Device(const char* mac_str, unsigned size, Error& ec)
{
	if(!mac_.set(mac_str, size))
	{
		ec = Error::invalid_value;
		return;
	}
	name_ = mac_str;
}

Device::Device(device_id id, mesh_addr_t const& host)
	: id_(id), mac_(host), name_(""), fw_version_(""),
	  hw_version_(""){}

Device::Device(device_id id, mesh_addr_t const& addr,
		mesh_addr_t const& mac_ap,
		mesh_addr_t const& parent,
		endpoint const& ep,
		std::string const& name/* = "" */,
		std::string const& fw_version/* = "" */,
		std::string const& hw_version/* = "" */,
		uint8_t channel_config/* = 0 */, uint8_t channel/* = 0 */,
		bool has_rtc/* = false */, bool has_temp_sensor/* = false */,
		int layer)
	: id_(id), mac_(addr), parent_(parent),
	  mac_ap_(mac_ap), ep_(ep), name_(name),
	  ch_config_(channel_config), ch_conn_(channel), layer_(layer),
	  fw_version_(fw_version), hw_version_(hw_version),
	  has_rtc_(has_rtc), has_sensor_temp_(has_temp_sensor){}

device_id Device::id() const noexcept
{
	return id_;
}

Net const* Device::net() const noexcept
{
	return net_;
}

void Device::net(Net const* net_ptr) noexcept
{
	net_ = net_ptr;
}

mesh_addr_t const& Device::mac() const noexcept
{
	return mac_;
}

mesh_addr_t const& Device::parent() const noexcept
{
	return parent_;
}

void Device::parent(mesh_addr_t const& addr) noexcept
{
	parent_ = addr;
}

mesh_addr_t const& Device::mac_ap() const noexcept
{
	return mac_ap_;
}

void Device::mac_ap(mesh_addr_t const& addr) noexcept
{
	mac_ap_ = addr;
}

mesh_addr_t const& Device::net_id() const noexcept
{
	return net_id_;
}

void Device::net_id(mesh_addr_t const& nid) noexcept
{
	net_id_ = nid;
}

/**
 * Endpoint
 */
endpoint const& Device::get_endpoint() const noexcept
{
	return ep_;
}

void Device::update_endpoint(endpoint const& ep) noexcept
{
	ep_ = ep;
}

std::string Device::name() const noexcept
{
	return name_;
}

void Device::name(std::string const& name) noexcept
{
	name_ = name;
}

std::uint8_t Device::channel_config() const noexcept
{
	return ch_config_;
}

std::uint8_t Device::channel() const noexcept
{
	return ch_conn_;
}

void Device::channel(uint8_t ch_cfg, uint8_t ch) noexcept
{
	ch_config_ = ch_cfg;
	ch_conn_ = ch;
}

int Device::layer() const noexcept
{
	return layer_;
}

void Device::layer(int nlayer) noexcept
{
	layer_ = nlayer;
}

std::vector<mesh_addr_t> const& Device::children_table() const noexcept
{
	return children_table_;
}

std::string const& Device::firmware_version() const noexcept
{
	return fw_version_;
}

std::string const& Device::hardware_version() const noexcept
{
	return hw_version_;
}

void Device::version(std::string const& fw, std::string const hw) noexcept
{
	fw_version_ = fw;
	hw_version_ = hw;
}

bool Device::has_rtc() const noexcept
{
	return has_rtc_;
}

bool Device::has_temperature_sensor() const noexcept
{
	return has_sensor_temp_;
}

void Device::has_config(bool rtc, bool temp_sensor) noexcept
{
	has_rtc_ = rtc;
	has_sensor_temp_ = temp_sensor;
}

std::uint32_t Device::last_packet_time() const noexcept
{
	return last_packet_time_;
}

void Device::update_last_packet_time() noexcept
{
	last_packet_time_ = static_cast<std::uint32_t>(time_epoch_miliseconds());
}

Sensor::Value<value_time> const& Device::rtc_time() const noexcept
{
	return rtc_time_;
}

std::int32_t Device::fuse() const noexcept
{
	return fuse_;
}

void Device::fuse(std::int32_t f) noexcept
{
	fuse_ = f;
}

void Device::jobs(std::uint8_t const* jobs, std::size_t size) noexcept
{
	if(size % job::packet_size) return;

	jobs_.clear();
	std::size_t offset = 0;
	while(size)
	{
		jobs_.emplace_back(
				reinterpret_cast<job_packet const*>(jobs + offset),
				reinterpret_cast<job_packet_executor const*>(jobs + offset + sizeof(job_packet)));
		size -= job::packet_size;
		offset += job::packet_size;
	}
}

std::vector<job> const& Device::jobs() const noexcept
{
	return jobs_;
}

std::vector<app> const& Device::apps() const noexcept
{
	return apps_;
}

Sensor::Value<int64_t> const& Device::uptime() const noexcept
{
	return uptime_;
}

Sensor::Sensor_List const& Device::sensor_list() const noexcept
{
	return slist_;
}

void Device::clear_children() noexcept
{
	children_table_.clear();
}

void Device::add_children(mesh_addr_t const& child) noexcept
{
	children_table_.push_back(child);
}

bool Device::operator==(Device const& rhs) const noexcept
{
	return mac_ == rhs.mac_;
}

//void Device::update(endpoint const& ep, Resource::config const& cfg, Net* net) noexcept
//{
//	process_packet(ep);
//
//	ch_config_ = cfg.channel_config;
//	ch_conn_ = cfg.channel_conn;
//	mac_ap_ = cfg.mac_ap;
//	net_id_ = cfg.net_id;
//	net_ = net;
//}

//void Device::update(endpoint const& ep, Resource::route const& route,
//		const uint8_t* children, std::size_t children_size) noexcept
//{
//	process_packet(ep);
//
//	layer_ = static_cast<int>(ntohs(route.layer));
//	parent_ = route.parent;
//
//	children_table_.clear();
//	for(std::size_t i = 0; i < children_size; i += 6)
//	{
//		children_table_.emplace_back(children);
//		children += 6;
//	}
//}

//void Device::update(endpoint const& ep, Resource::full_config const& cfg, Net* net,
//		const uint8_t* children, std::size_t children_size) noexcept
//{
//	process_packet(ep);
//
//	ch_config_ = cfg.fconfig.channel_config;
//	ch_conn_ = cfg.fconfig.channel_conn;
//	mac_ap_ = cfg.fconfig.mac_ap;
//	net_id_ = cfg.fconfig.net_id;
//	net_ = net;
//
//	layer_ = static_cast<int>(ntohs(cfg.froute.layer));
//	parent_ = cfg.froute.parent;
//
//	children_table_.clear();
//	for(std::size_t i = 0; i < children_size; i += 6)
//	{
//		children_table_.emplace_back(children);
//		children += 6;
//	}
//}

//void Device::update(endpoint const& ep, Resource::board_config const& cfg,
//		const char* version, std::size_t version_len) noexcept
//{
//	process_packet(ep);
//
//	has_rtc_ = cfg.has_rtc;
//	has_sensor_temp_ = cfg.has_temp_sensor;
//
//	std::string str{static_cast<const char*>(version), version_len};
//	std::string::size_type pos = str.find("|");
//	if(pos != std::string::npos)
//	{
//		fw_version_ = str.substr(0, pos);
//		hw_version_ = str.substr(pos + 1);
//	}
//	else
//	{
//		fw_version_ = {static_cast<const char*>(version), version_len};
//		hw_version_ = "";
//	}
//}

bool Device::update_sensor(unsigned type, unsigned index,
				value_time time, Sensor::sensor_value const& value,
				bool add_change /* = false */) noexcept
{
//	last_packet_time_ = static_cast<std::uint32_t>(time_epoch_miliseconds());
	return slist_.add(type, index, time, value, add_change);
}

int Device::update_ac_load(unsigned index, bool value) noexcept
{
	if(index > 2) return false;
	Sensor::Sensor_List::value& list = slist_.get_or_add(GPIOS_ARRAY_SENSOR_TYPE, GPIOS_ARRAY_SENSOR_INDEX);
	int nindex = index + 8;
	if(list.size() == 0)
	{
		int gpios_value = value << nindex;
		list.add(gpios_value);
		return gpios_value;
	}
	int v = list[static_cast<unsigned>(list.size() - 1)].value,
		nvalue = (v & ~(1UL << nindex)) | (value << nindex);
//	list.add_change((v & ~(1UL << nindex)) | (value << nindex));
	list.add(nvalue);

	return nvalue;
}

void Device::update_uptime(int64_t time) noexcept
{
	uptime_.update(time);
}

void Device::update_rtc_time(value_time time) noexcept
{
	rtc_time_.update(time);
}

/**
 * Apps
 */
void Device::add_app(std::string const& name, std::size_t size, const sha256_hash hash) noexcept
{
	auto it = std::find_if(apps_.begin(), apps_.end(), [&name](app const& v){ return v.name == name; });
	if(it == apps_.end())
	{
		apps_.emplace_back(name, size, hash);
		return;
	}
	it->size = size;
	std::memcpy(it->hash, hash, sizeof(sha256_hash));
}

void Device::clear_apps() noexcept
{
	apps_.clear();
}

void Device::delete_app(std::string const& name) noexcept
{
	apps_.erase(std::remove_if(apps_.begin(), apps_.end(), [&name](app const& v){ return v.name == name; }), apps_.end());
}

//void Device::process_packet(endpoint const& ep) noexcept
//{
//	update_endpoint(ep);
//	last_packet_time_ = static_cast<std::uint32_t>(time_epoch_miliseconds());
//}

}//Device
}//Agro
