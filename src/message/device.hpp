#ifndef AGRO_DAEMON_MAKE_DEVICE_MESSAGE_HPP__
#define AGRO_DAEMON_MAKE_DEVICE_MESSAGE_HPP__

#include "rapidjson/document.h"
#include "../device/device.hpp"
#include "../device/list.hpp"
#include <string>

namespace Message{

void device_config_to_json(rapidjson::Document& doc, Device const& dev) noexcept;
std::string device_config_to_json(Device const& dev) noexcept;

void device_status_to_json(rapidjson::Document& doc, Device const& dev) noexcept;
std::string device_status_to_json(Device const& dev) noexcept;

void device_route_to_json(rapidjson::Document& doc, Device const& dev) noexcept;
std::string device_route_to_json(Device const& dev) noexcept;

void device_full_config_to_json(rapidjson::Document& doc, Device const& dev) noexcept;
std::string device_full_config_to_json(Device const& dev) noexcept;

void device_board_to_json(rapidjson::Document& doc, Device const& dev) noexcept;
std::string device_board_to_json(Device const& dev) noexcept;

void device_sensor_data_to_json(rapidjson::Document& doc, Device const& dev) noexcept;
std::string device_sensor_data_to_json(Device const& dev) noexcept;

void device_gpios_to_json(rapidjson::Document& doc, Device const& dev) noexcept;
std::string device_gpios_to_json(Device const& dev) noexcept;

void device_to_json(rapidjson::Document& doc, Device const& dev) noexcept;
std::string device_to_json(Device const& dev) noexcept;

void device_list_to_json(rapidjson::Document& doc, Device_List const& list) noexcept;
std::string device_list_to_json(Device_List const& list) noexcept;

}

#endif /* AGRO_DAEMON_MAKE_DEVICE_MESSAGE_HPP__ */
