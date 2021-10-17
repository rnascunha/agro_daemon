#include "process.hpp"
#include "types.hpp"
#include "../../error.hpp"
#include "../../instance/agro.hpp"
#include "../message/device.hpp"

namespace Agro{
namespace Device{
namespace Resource{

bool process_status(Agro::Device::Device& device,
					Agro::instance& instance,
					engine::endpoint const& ep,
					const void* payload, std::size_t payload_len,
					std::error_code& ec) noexcept
{
	if(payload_len < sizeof(status))
	{
		ec = make_error_code(Error::value_too_small);
		return false;
	}
	status const* rt = static_cast<status const*>(payload);

	device.update(ep, *rt);

	instance.share()->write_all_policy(Agro::Authorization::rule::view_device,
			std::make_shared<std::string>(Agro::Device::Message::device_status_to_json(device)));

	return true;
}

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
	route const* rt = static_cast<route const*>(payload);

	device.update(ep, *rt,
			static_cast<const uint8_t*>(payload) + sizeof(route),
			payload_len - sizeof(route));

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

	device.update(ep, *rt,
				static_cast<const char*>(payload) + sizeof(board_config),
				payload_len - sizeof(board_config));

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
	config const* rt = static_cast<config const*>(payload);

	::mesh_addr_t net_addr{rt->net_id.addr};
	Agro::Device::Net* net = instance.get_or_add_net(net_addr);
	device.update(ep, *rt, net);

	instance.share()->write_all_policy(Agro::Authorization::rule::view_device,
			std::make_shared<std::string>(Agro::Device::Message::device_config_to_json(device)));

	return true;
}

bool process_full_config(Agro::Device::Device& device,
					Agro::instance& instance,
					engine::endpoint const& ep,
					const void* payload, std::size_t payload_len,
					std::error_code& ec) noexcept
{
	if(payload_len < sizeof(full_config))
	{
		ec = make_error_code(Error::value_too_small);
		return false;
	}
	full_config const* rt = static_cast<full_config const*>(payload);

	::mesh_addr_t net_addr{rt->fconfig.net_id.addr};
	Agro::Device::Net* net = instance.get_or_add_net(net_addr);

	device.update(ep, *rt, net,
			static_cast<const uint8_t*>(payload) + sizeof(route) + sizeof(status) + sizeof(config),
			payload_len - sizeof(route) - sizeof(status) - sizeof(config));

	instance.share()->write_all_policy(Agro::Authorization::rule::view_device,
			std::make_shared<std::string>(Agro::Device::Message::device_full_config_to_json(device)));

	instance.update_tree(device);

	return true;
}

bool process_sensor_data(Agro::Device::Device& device,
					Agro::instance& instance,
					engine::endpoint const& ep,
					const void* payload, std::size_t payload_len,
					std::error_code& ec) noexcept
{
	if(payload_len < sizeof(sensor_data))
	{
		ec = make_error_code(Error::value_too_small);
		return false;
	}
	sensor_data const* rt = static_cast<sensor_data const*>(payload);

	device.update(ep, *rt);

	instance.share()->write_all_policy(Agro::Authorization::rule::view_device,
			std::make_shared<std::string>(Agro::Device::Message::device_sensor_data_to_json(device)));

	return true;
}

}//Resource
}//Device
}//Agro
