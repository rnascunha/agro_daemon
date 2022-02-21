#ifndef AGRO_MESH_DEVICE_HELPER_HPP__
#define AGRO_MESH_DEVICE_HELPER_HPP__

#include "types.hpp"
#include "device.hpp"

#ifndef MACSTR
#define MACSTR "%02x:%02x:%02x:%02x:%02x:%02x"
#endif /* MACSTR */
#ifndef MAC2STR
#define MAC2STR(a) (a)[0], (a)[1], (a)[2], (a)[3], (a)[4], (a)[5]
#endif /* MAC2STR */

bool string_to_mac(const char* str, unsigned size, mesh_addr_t& addr) noexcept;
void read_packet_app_list(Agro::Device::Device&, const void*, std::size_t) noexcept;

mesh_addr_t mac_ap_to_addr(mesh_addr_t const&) noexcept;

#endif /* AGRO_MESH_DEVICE_HELPER_HPP__ */
