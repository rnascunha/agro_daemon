#ifndef AGRO_DAEMON_DEVICE_RESOURCES_TYPES_HPP__
#define AGRO_DAEMON_DEVICE_RESOURCES_TYPES_HPP__

#include <cstdint>

namespace Agro{
namespace Device{
namespace Resource{

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

struct __attribute__((packed)) board_config{
	std::uint8_t	has_temp_sensor:1;
	std::uint8_t	has_rtc:1;
	std::uint8_t	:6;
};

struct __attribute__((packed)) sensor_data{
	std::uint32_t	time;
	float			temp;
	std::uint8_t	wl1:1;
	std::uint8_t	wl2:1;
	std::uint8_t	wl3:1;
	std::uint8_t	wl4:1;
	std::uint8_t	gp1:1;
	std::uint8_t	gp2:1;
	std::uint8_t	gp3:1;
	std::uint8_t	gp4:1;
	std::uint8_t	ac1:1;
	std::uint8_t	ac2:1;
	std::uint8_t	ac3:1;
	std::uint8_t	:5;
	int8_t			rssi;
};

}//Resource
}//Device
}//Agro

#endif /* AGRO_DAEMON_DEVICE_RESOURCES_TYPES_HPP__ */