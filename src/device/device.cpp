#include "device.hpp"
#include "../helper/time_helper.hpp"

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

mesh_addr_t const& Device::mac_ap() const noexcept
{
	return mac_ap_;
}

mesh_addr_t const& Device::net_id() const noexcept
{
	return net_id_;
}

endpoint const& Device::get_endpoint() const noexcept
{
	return ep_;
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

int Device::layer() const noexcept
{
	return layer_;
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

Value_List<std::int8_t> const& Device::rssi() const noexcept
{
	return rssi_;
}

bool Device::has_rtc() const noexcept
{
	return has_rtc_;
}

bool Device::has_temperature_sensor() const noexcept
{
	return has_sensor_temp_;
}

std::uint32_t Device::last_packet_time() const noexcept
{
	return last_packet_time_;
}

Value_List<float> const& Device::temperature() const noexcept
{
	return temp_;
}

Value_List<std::uint8_t> const& Device::gpios() const noexcept
{
	return gpios_;
}

Value_List<std::uint8_t> const& Device::gpios_out() const noexcept
{
	return gpios_out_;
}

Value<value_time> const& Device::rtc_time() const noexcept
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
		jobs_.emplace_back(jobs + offset);
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

Value<int64_t> const& Device::uptime() const noexcept
{
	return uptime_;
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

void Device::update(endpoint const& ep, Resource::status const& sts) noexcept
{
	process_packet(ep);

	rssi_.add(sts.rssi);
}

void Device::update(endpoint const& ep, Resource::config const& cfg, Net* net) noexcept
{
	process_packet(ep);

	ch_config_ = cfg.channel_config;
	ch_conn_ = cfg.channel_conn;
	mac_ap_ = cfg.mac_ap;
	net_id_ = cfg.net_id;
	net_ = net;
}

void Device::update(endpoint const& ep, Resource::route const& route,
		const uint8_t* children, std::size_t children_size) noexcept
{
	process_packet(ep);

	layer_ = static_cast<int>(ntohs(route.layer));
	parent_ = route.parent;

	children_table_.clear();
	for(std::size_t i = 0; i < children_size; i += 6)
	{
		children_table_.emplace_back(children);
		children += 6;
	}
}

void Device::update(endpoint const& ep, Resource::full_config const& cfg, Net* net,
		const uint8_t* children, std::size_t children_size) noexcept
{
	process_packet(ep);

	rssi_.add(cfg.fstatus.rssi);

	ch_config_ = cfg.fconfig.channel_config;
	ch_conn_ = cfg.fconfig.channel_conn;
	mac_ap_ = cfg.fconfig.mac_ap;
	net_id_ = cfg.fconfig.net_id;
	net_ = net;

	layer_ = static_cast<int>(ntohs(cfg.froute.layer));
	parent_ = cfg.froute.parent;

	children_table_.clear();
	for(std::size_t i = 0; i < children_size; i += 6)
	{
		children_table_.emplace_back(children);
		children += 6;
	}
}

void Device::update(endpoint const& ep, Resource::board_config const& cfg,
		const char* version, std::size_t version_len) noexcept
{
	process_packet(ep);

	has_rtc_ = cfg.has_rtc;
	has_sensor_temp_ = cfg.has_temp_sensor;

	std::string str{static_cast<const char*>(version), version_len};
	std::string::size_type pos = str.find("|");
	if(pos != std::string::npos)
	{
		fw_version_ = str.substr(0, pos);
		hw_version_ = str.substr(pos + 1);
	}
	else
	{
		fw_version_ = {static_cast<const char*>(version), version_len};
		hw_version_ = "";
	}
}

void Device::update(endpoint const& ep, Resource::sensor_data const& data) noexcept
{
	process_packet(ep);

	std::uint8_t gpios_value = data.wl1 | (data.wl2 << 1) |
								(data.wl3 << 2) | (data.wl4 << 3) |
								(data.gp1 << 4) | (data.gp2 << 5) |
								(data.gp3 << 6) | (data.gp4 << 7);
	std::uint8_t gpios_out_value = data.ac1 | (data.ac2 << 1) |
									(data.ac3 << 2);

	if(has_rtc_)
	{
		rssi_.add(data.time, data.rssi);
		if(data.temp != -127.0)	//Invalid read
			temp_.add(data.time, data.temp);

		gpios_.add_change(data.time, gpios_value);
		gpios_out_.add_change(data.time, gpios_out_value);
	}
	else
	{
		rssi_.add(data.rssi);
		if(data.temp != -127.0)	//Invalid read
			temp_.add(data.temp);

		gpios_.add_change(gpios_value);
		gpios_out_.add_change(gpios_out_value);
	}
}

bool Device::update_ac_load(unsigned index, bool value) noexcept
{
	if(index > 2) return false;
	if(gpios_out_.size() == 0)
	{
		std::uint8_t gpios_value = value << index;
		gpios_out_.add(gpios_value);
		return true;
	}
	std::uint8_t v = gpios_out_[gpios_out_.size() - 1].value;
	gpios_out_.add_change((v & ~(1UL << index)) | (value << index));

	return true;
}

void Device::update_uptime(int64_t time) noexcept
{
	uptime_.update(time);
}

void Device::update_rtc_time(value_time time) noexcept
{
	rtc_time_.update(time);
}

void Device::update_endpoint(endpoint const& ep) noexcept
{
	ep_ = ep;
}

void Device::add_app(std::string const& name, std::size_t size) noexcept
{
	auto it = std::find_if(apps_.begin(), apps_.end(), [&name](app const& v){ return v.name == name; });
	if(it == apps_.end())
	{
		apps_.emplace_back(name, size);
		return;
	}
	it->size = size;
}

void Device::delete_app(std::string const& name) noexcept
{
	apps_.erase(std::remove_if(apps_.begin(), apps_.end(), [&name](app const& v){ return v.name == name; }), apps_.end());
}

void Device::process_packet(endpoint const& ep) noexcept
{
	update_endpoint(ep);
	last_packet_time_ = static_cast<std::uint32_t>(time_epoch_miliseconds());
}

}//Device
}//Agro
