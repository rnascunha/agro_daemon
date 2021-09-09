#include "process.hpp"
#include "types.hpp"
#include "../error.hpp"
#include "../message/device.hpp"

namespace Resource{

bool process_status(Device_List& list,
					Agro::share_ptr data_share,
					engine::endpoint const& ep,
					CoAP::Message::Option::option const& uri_host,
					const void* payload, std::size_t payload_len,
					std::error_code& ec) noexcept
{
	if(payload_len < sizeof(status))
	{
		ec = make_error_code(Error::value_too_small);
		return false;
	}
	status const* rt = static_cast<status const*>(payload);

	::mesh_addr_t host{static_cast<const char*>(uri_host.value), uri_host.length, ec};
	if(ec) return false;

	auto& dev = list.update(host, ep, *rt);

	data_share->write_all(std::make_shared<std::string>(Message::device_status_to_json(dev)));

	return true;
}

bool process_route(Device_List& list,
					Agro::share_ptr data_share,
					engine::endpoint const& ep,
					CoAP::Message::Option::option const& uri_host,
					const void* payload, std::size_t payload_len,
					std::error_code& ec) noexcept
{
	if(payload_len < sizeof(route))
	{
		ec = make_error_code(Error::value_too_small);
		return false;
	}
	route const* rt = static_cast<route const*>(payload);

	::mesh_addr_t host{static_cast<const char*>(uri_host.value), uri_host.length, ec};
	if(ec) return false;

	auto& dev = list.update(host, ep, *rt,
							static_cast<const uint8_t*>(payload) + sizeof(route),
							payload_len - sizeof(route));

	data_share->write_all(std::make_shared<std::string>(Message::device_route_to_json(dev)));

	return true;
}

bool process_board(Device_List& list,
					Agro::share_ptr data_share,
					engine::endpoint const& ep,
					CoAP::Message::Option::option const& uri_host,
					const void* payload, std::size_t payload_len,
					std::error_code& ec) noexcept
{
	if(payload_len < sizeof(board_config))
	{
		ec = make_error_code(Error::value_too_small);
		return false;
	}
	board_config const* rt = static_cast<board_config const*>(payload);

	::mesh_addr_t host{static_cast<const char*>(uri_host.value), uri_host.length, ec};
	if(ec) return false;

	auto& dev = list.update(host, ep, *rt,
							static_cast<const char*>(payload) + sizeof(board_config),
							payload_len - sizeof(board_config));

	data_share->write_all(std::make_shared<std::string>(Message::device_board_to_json(dev)));

	return true;
}

bool process_config(Device_List& list,
					Agro::share_ptr data_share,
					engine::endpoint const& ep,
					CoAP::Message::Option::option const& uri_host,
					const void* payload, std::size_t payload_len,
					std::error_code& ec) noexcept
{
	if(payload_len < sizeof(config))
	{
		ec = make_error_code(Error::value_too_small);
		return false;
	}
	config const* rt = static_cast<config const*>(payload);

	::mesh_addr_t host{static_cast<const char*>(uri_host.value), uri_host.length, ec};
	if(ec) return false;

	auto& dev = list.update(host, ep, *rt);

	data_share->write_all(std::make_shared<std::string>(Message::device_config_to_json(dev)));

	return true;
}

bool process_full_config(Device_List& list,
					Agro::share_ptr data_share,
					engine::endpoint const& ep,
					CoAP::Message::Option::option const& uri_host,
					const void* payload, std::size_t payload_len,
					std::error_code& ec) noexcept
{
	if(payload_len < sizeof(full_config))
	{
		ec = make_error_code(Error::value_too_small);
		return false;
	}
	full_config const* rt = static_cast<full_config const*>(payload);

	::mesh_addr_t host{static_cast<const char*>(uri_host.value), uri_host.length, ec};
	if(ec) return false;

	auto& dev = list.update(host, ep, *rt,
			static_cast<const uint8_t*>(payload) + sizeof(route) + sizeof(status) + sizeof(config),
			payload_len - sizeof(route) - sizeof(status) - sizeof(config));

	data_share->write_all(std::make_shared<std::string>(Message::device_full_config_to_json(dev)));

	return true;
}

bool process_sensor_data(Device_List& list,
					Agro::share_ptr data_share,
					engine::endpoint const& ep,
					CoAP::Message::Option::option const& uri_host,
					const void* payload, std::size_t payload_len,
					std::error_code& ec) noexcept
{
	if(payload_len < sizeof(sensor_data))
	{
		ec = make_error_code(Error::value_too_small);
		return false;
	}
	sensor_data const* rt = static_cast<sensor_data const*>(payload);

	::mesh_addr_t host{static_cast<const char*>(uri_host.value), uri_host.length, ec};
	if(ec) return false;

	auto& dev = list.update(host, ep, *rt);

	data_share->write_all(std::make_shared<std::string>(Message::device_sensor_data_to_json(dev)));

	return true;
}

}//Resource
