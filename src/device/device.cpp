#include "device.hpp"

Device::Device(){}

Device::Device(mesh_addr_t const& mac)
	: mac_(mac){}

Device::Device(const char* mac_str, unsigned size, Error& ec)
{
	if(!mac_.set(mac_str, size))
	{
		ec = Error::invalid_value;
	}
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

std::uint8_t Device::channel_config() const noexcept
{
	return ch_config_;
}

std::uint8_t Device::channel() const noexcept
{
	return ch_conn_;
}

std::uint8_t Device::layer() const noexcept
{
	return layer_;
}

std::vector<mesh_addr_t> const& Device::children_table() const noexcept
{
	return children_table_;
}

std::vector<mesh_addr_t> const& Device::children() const noexcept
{
	return children_;
}

std::string Device::firmware_version() const noexcept
{
	return fw_version_;
}

std::string Device::hardware_version() const noexcept
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

void Device::update(endpoint const& ep, Resource::status const& sts) noexcept
{
	update_endpoint(ep);
	rssi_.add(sts.rssi);
}

void Device::update(endpoint const& ep, Resource::config const& cfg) noexcept
{
	update_endpoint(ep);

	ch_config_ = cfg.channel_config;
	ch_conn_ = cfg.channel_conn;
	mac_ap_ = cfg.mac_ap;
	net_id_ = cfg.net_id;
}

void Device::update(endpoint const& ep, Resource::route const& route,
		const uint8_t* children, std::size_t children_size) noexcept
{
	update_endpoint(ep);

	layer_ = route.layer;
	parent_ = route.parent;

	children_table_.clear();
	for(std::size_t i = 0; i < children_size; i += 6)
	{
		children_table_.emplace_back(children);
		children += 6;
	}
}

void Device::update(endpoint const& ep, Resource::full_config const& cfg,
		const uint8_t* children, std::size_t children_size) noexcept
{
	rssi_.add(cfg.fstatus.rssi);

	ch_config_ = cfg.fconfig.channel_config;
	ch_conn_ = cfg.fconfig.channel_conn;
	mac_ap_ = cfg.fconfig.mac_ap;
	net_id_ = cfg.fconfig.net_id;

	layer_ = cfg.froute.layer;
	parent_ = cfg.froute.parent;

	children_table_.clear();
	for(std::size_t i = 0; i < children_size; i += 6)
	{
		children_table_.emplace_back(children);
		children += 6;
	}
}

void Device::update(endpoint const&, Resource::board_config const& cfg,
		const char* version, std::size_t version_len) noexcept
{
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
		fw_version_ = std::string{static_cast<const char*>(version), version_len};
		hw_version_ = "";
	}
}

void Device::update_endpoint(endpoint const& ep) noexcept
{
	ep_ = ep;
}
