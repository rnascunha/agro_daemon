#ifndef AGRO_DAEMON_RESOURCE_PROCESS_HPP__
#define AGRO_DAEMON_RESOURCE_PROCESS_HPP__

#include <cstdlib>
#include <system_error>
#include "coap-te.hpp"
#include "../device/device.hpp"
#include "../websocket/types.hpp"

namespace Resource{

bool process_status(Agro::Device::Device&,
					Agro::share_ptr,
					engine::endpoint const&,
					const void* payload, std::size_t payload_len,
					std::error_code&) noexcept;

bool process_route(Agro::Device::Device&,
					Agro::share_ptr,
					Agro::instance& instance,
					engine::endpoint const&,
					const void* payload, std::size_t payload_len,
					std::error_code&) noexcept;

bool process_board(Agro::Device::Device&,
					Agro::share_ptr,
					engine::endpoint const&,
					const void* payload, std::size_t payload_len,
					std::error_code&) noexcept;

bool process_config(Agro::Device::Device&,
					Agro::share_ptr,
					Agro::instance&,
					engine::endpoint const&,
					const void* payload, std::size_t payload_len,
					std::error_code&) noexcept;

bool process_full_config(Agro::Device::Device&,
					Agro::share_ptr,
					Agro::instance&,
					engine::endpoint const&,
					const void* payload, std::size_t payload_len,
					std::error_code&) noexcept;

bool process_sensor_data(Agro::Device::Device&,
					Agro::share_ptr,
					engine::endpoint const&,
					const void* payload, std::size_t payload_len,
					std::error_code&) noexcept;

}//Resource

#endif /* AGRO_DAEMON_RESOURCE_PROCESS_HPP__ */
