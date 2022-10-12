#include "process.hpp"
#include "types.hpp"
#include "../../error.hpp"
#include "../../instance/agro.hpp"
#include "../message/device.hpp"
#include "../helper.hpp"

namespace Agro{
namespace Device{
namespace Resource{

static void process_packet(Device& device, endpoint ep) noexcept;
static void update_config_data(Device& device,
		config const& data,
		Agro::instance& instance) noexcept;
static void update_route_data(Device& device,
			route const& data,
			std::uint8_t const* children, std::size_t children_size,
			Agro::instance& instance) noexcept;

bool process_route(Agro::Device::Device& device,
					Agro::instance& instance,
					engine::endpoint const& ep,
					const void* payload, std::size_t payload_len,
					std::error_code& ec,
					bool force_send /* = true */) noexcept
{
	if(payload_len < sizeof(route))
	{
		ec = make_error_code(Error::value_too_small);
		return false;
	}

	process_packet(device, ep);
	update_route_data(device, *static_cast<route const*>(payload),
			static_cast<const uint8_t*>(payload) + sizeof(route),
			payload_len - sizeof(route),
			instance);

	if(instance.update_tree(device) || force_send)
	{
		instance.share()->write_all_policy(Agro::Authorization::rule::view_device,
						std::make_shared<std::string>(Agro::Device::Message::device_route_to_json(device)));
	}

	return true;
}

bool process_board(Agro::Device::Device& device,
					Agro::instance& instance,
					engine::endpoint const& ep,
					const void* payload, std::size_t payload_len,
					std::error_code& ec) noexcept
{
	if(payload_len < sizeof(board_config))
	{
		ec = make_error_code(Error::value_too_small);
		return false;
	}
	board_config const* rt = static_cast<board_config const*>(payload);

	process_packet(device, ep);

	device.has_config(rt->has_rtc, rt->has_temp_sensor);
	std::string str{static_cast<const char*>(payload) + sizeof(board_config),	//Version*
					payload_len - sizeof(board_config)};						//Version length

	std::string::size_type pos = str.find("|");
	pos != std::string::npos ?
		device.version(str.substr(0, pos), str.substr(pos + 1)) :
		device.version(str, "");

	instance.share()->write_all_policy(Agro::Authorization::rule::view_device,
			std::make_shared<std::string>(Agro::Device::Message::device_board_to_json(device)));

	return true;
}

bool process_config(Agro::Device::Device& device,
					Agro::instance& instance,
					engine::endpoint const& ep,
					const void* payload, std::size_t payload_len,
					std::error_code& ec) noexcept
{
	if(payload_len < sizeof(config))
	{
		ec = make_error_code(Error::value_too_small);
		return false;
	}

	process_packet(device, ep);
	update_config_data(device, *static_cast<config const*>(payload), instance);

	instance.share()->write_all_policy(Agro::Authorization::rule::view_device,
			std::make_shared<std::string>(Agro::Device::Message::device_config_to_json(device)));

	return true;
}

bool process_full_config(Agro::Device::Device& device,
					Agro::instance& instance,
					engine::endpoint const&,
					const void* payload, std::size_t payload_len,
					std::error_code& ec) noexcept
{
	if(payload_len < sizeof(full_config))
	{
		ec = make_error_code(Error::value_too_small);
		return false;
	}
	full_config const* rt = static_cast<full_config const*>(payload);

	update_config_data(device, rt->fconfig, instance);
	update_route_data(device, rt->froute,
			static_cast<const uint8_t*>(payload) + sizeof(route) + sizeof(int8_t) + sizeof(config),
			payload_len - sizeof(route) - sizeof(int8_t) - sizeof(config),
			instance);

	instance.share()->write_all_policy(Agro::Authorization::rule::view_device,
			std::make_shared<std::string>(Agro::Device::Message::device_full_config_to_json(device)));

	instance.update_tree(device);

	return true;
}

void process_sensors_data(Agro::Device::Device& device,
					Agro::instance& instance,
					engine::endpoint const& ep,
					const void* payload, std::size_t payload_len,
					std::error_code&) noexcept
{
	process_packet(device, ep);

	instance.update_db_device(device);
	instance.update_sensor_value(device, payload, payload_len);

	instance.share()->write_all_policy(Agro::Authorization::rule::view_device,
			std::make_shared<std::string>(
					Message::device_sensor_data(device,
							payload, payload_len,
							instance.sensor_list())));
}

static void process_packet(Device& device, endpoint ep) noexcept
{
	device.update_last_packet_time();
	device.update_endpoint(ep);
}

static void update_config_data(Device& device,
		config const& data,
		Agro::instance& instance) noexcept
{
	device.channel(data.channel_config, data.channel_conn);
	device.mac_ap(data.mac_ap.addr);
	device.net_id(data.net_id.addr);
	device.net(instance.get_or_add_net(::mesh_addr_t{data.net_id.addr}));
}

static void initiate_child_data(Device const& parent, Device& child) noexcept
{
	child.layer(parent.layer() + 1);
	child.parent(parent.mac());
	child.update_endpoint(parent.get_endpoint());
	child.net_id(parent.net_id());
	child.net(parent.net());
}

static void update_route_data(Device& device,
			route const& data,
			std::uint8_t const* children, std::size_t children_size,
			Agro::instance& instance) noexcept
{
	device.layer(static_cast<int>(ntohs(data.layer)));
	device.parent(device.layer() != 1 ? mac_ap_to_addr(data.parent.addr) : data.parent.addr);

	device.clear_children();
	std::vector<Device const*> new_device;
	for(std::size_t i = 0; i < children_size; i += 6)
	{
		device.add_children(children);
		auto ret = instance.add_or_get_device(children);
		//Is new device
		if(ret.second)
		{
			initiate_child_data(device, *ret.first);
			new_device.push_back(ret.first);
		}
		children += 6;
	}
	instance.device_connected(new_device);
}

}//Resource
}//Device
}//Agro
