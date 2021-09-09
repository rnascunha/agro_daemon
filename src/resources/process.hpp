#ifndef AGRO_DAEMON_RESOURCE_PROCESS_HPP__
#define AGRO_DAEMON_RESOURCE_PROCESS_HPP__

#include <cstdlib>
#include <system_error>
#include "coap-te.hpp"
#include "../device/list.hpp"
#include "../websocket/types.hpp"

namespace Resource{

bool process_status(Device_List&,
					Agro::share_ptr,
					engine::endpoint const&,
					CoAP::Message::Option::option const& uri_host,
					const void* payload, std::size_t payload_len,
					std::error_code&) noexcept;

bool process_route(Device_List&,
					Agro::share_ptr,
					engine::endpoint const&,
					CoAP::Message::Option::option const& uri_host,
					const void* payload, std::size_t payload_len,
					std::error_code&) noexcept;

bool process_board(Device_List&,
					Agro::share_ptr,
					engine::endpoint const&,
					CoAP::Message::Option::option const& uri_host,
					const void* payload, std::size_t payload_len,
					std::error_code&) noexcept;

bool process_config(Device_List&,
					Agro::share_ptr,
					engine::endpoint const&,
					CoAP::Message::Option::option const& uri_host,
					const void* payload, std::size_t payload_len,
					std::error_code&) noexcept;

bool process_full_config(Device_List&,
					Agro::share_ptr,
					engine::endpoint const&,
					CoAP::Message::Option::option const& uri_host,
					const void* payload, std::size_t payload_len,
					std::error_code&) noexcept;

bool process_sensor_data(Device_List&,
					Agro::share_ptr,
					engine::endpoint const&,
					CoAP::Message::Option::option const& uri_host,
					const void* payload, std::size_t payload_len,
					std::error_code&) noexcept;

}//Resource

#endif /* AGRO_DAEMON_RESOURCE_PROCESS_HPP__ */
