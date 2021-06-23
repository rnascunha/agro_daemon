#ifndef AGRO_DAEMON_NET_MESSAGES_TYPES_HPP__
#define AGRO_DAEMON_NET_MESSAGES_TYPES_HPP__

#include <cstdint>

#ifndef MACSTR
#define MACSTR "%02x:%02x:%02x:%02x:%02x:%02x"
#endif /* MACSTR */
#ifndef MAC2STR
#define MAC2STR(a) (a)[0], (a)[1], (a)[2], (a)[3], (a)[4], (a)[5]
#endif /* MAC2STR */

struct mesh_addr_t{
    std::uint8_t addr[6];    /**< mac address */
};

struct __attribute__((packed)) route{
	mesh_addr_t 	parent;
	std::uint16_t	layer;
};

struct __attribute__((packed)) config{
	mesh_addr_t		mac_ap;
	mesh_addr_t		net_id;
	uint8_t			channel_config:4;
	uint8_t			channel_conn:4;
};

struct __attribute__((packed)) status{
	int8_t			rssi;
};

struct __attribute__((packed)) full_config{
	config			fconfig;
	status			fstatus;
	route			froute;
};

#endif /* AGRO_DAEMON_NET_MESSAGES_TYPES_HPP__ */
