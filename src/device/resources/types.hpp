#ifndef AGRO_DAEMON_DEVICE_RESOURCES_TYPES_HPP__
#define AGRO_DAEMON_DEVICE_RESOURCES_TYPES_HPP__

#include <cstdint>
#include "../../helper/packed.h"

namespace Agro{
namespace Device{
namespace Resource{

struct mesh_addr_t{
	std::uint8_t addr[6];    /**< mac address */
};

PACK(struct route{
	mesh_addr_t 	parent;
	std::uint16_t	layer;
});

PACK(struct config{
	mesh_addr_t		mac_ap;
	mesh_addr_t		net_id;
	std::uint8_t	channel_config:4;
	std::uint8_t	channel_conn:4;
});

PACK(struct full_config{
	config			fconfig;
	std::int8_t		rssi;
	route			froute;
});

PACK(struct board_config{
	std::uint8_t	has_temp_sensor:1;
	std::uint8_t	has_rtc:1;
	std::uint8_t	:6;
});

// #undef PACK

}//Resource
}//Device
}//Agro

#endif /* AGRO_DAEMON_DEVICE_RESOURCES_TYPES_HPP__ */
