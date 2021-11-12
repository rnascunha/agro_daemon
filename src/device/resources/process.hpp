#ifndef AGRO_DAEMON_DEVICE_RESOURCE_PROCESS_HPP__
#define AGRO_DAEMON_DEVICE_RESOURCE_PROCESS_HPP__

#include <cstdlib>
#include <system_error>

#include "coap-te.hpp"
#include "../device.hpp"
#include "../../instance/agro.hpp"

namespace Agro{
namespace Device{
namespace Resource{

bool process_status(Agro::Device::Device&,
					Agro::instance&,
					engine::endpoint const&,
					const void* payload, std::size_t payload_len,
					std::error_code&) noexcept;

bool process_route(Agro::Device::Device&,
					Agro::instance&,
					engine::endpoint const&,
					const void* payload, std::size_t payload_len,
					std::error_code&,
					bool force_send = true) noexcept;

bool process_board(Agro::Device::Device&,
					Agro::instance&,
					engine::endpoint const&,
					const void* payload, std::size_t payload_len,
					std::error_code&) noexcept;

bool process_config(Agro::Device::Device&,
					Agro::instance&,
					engine::endpoint const&,
					const void* payload, std::size_t payload_len,
					std::error_code&) noexcept;

bool process_full_config(Agro::Device::Device&,
					Agro::instance&,
					engine::endpoint const&,
					const void* payload, std::size_t payload_len,
					std::error_code&) noexcept;

void process_sensors_data(Agro::Device::Device& device,
					Agro::instance& instance,
					engine::endpoint const& ep,
					const void* payload, std::size_t payload_len,
					std::error_code& ec) noexcept;

}//Resource
}//Device
}//Agro

#endif /* AGRO_DAEMON_DEVICE_RESOURCE_PROCESS_HPP__ */
