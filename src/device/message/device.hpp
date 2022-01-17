#ifndef AGRO_DAEMON_MAKE_DEVICE_MESSAGE_HPP__
#define AGRO_DAEMON_MAKE_DEVICE_MESSAGE_HPP__

#include "rapidjson/document.h"
#include "../device.hpp"
#include "../list.hpp"
#include "../tree.hpp"
//#include "../../message/types.hpp"
#include <string>
#include "../../sensor/message/sensor.hpp"
#include "../../helper/utility.hpp"

namespace Agro{
namespace Device{
namespace Message{

enum class device_commands{
	list = 0,
	data,
	tree,
	edit,
	request,
	custom_response,
};

constexpr const config<device_commands> device_config[] = {
	{device_commands::list, "list"},
	{device_commands::data, "data"},
	{device_commands::tree, "tree"},
	{device_commands::edit, "edit"},
	{device_commands::request, "request"},
	{device_commands::custom_response, "custom_response"},
};

inline constexpr auto get_device_config(const char* name) noexcept
{
    return ::get_config(name, device_config);
}

inline constexpr auto get_config(device_commands mtype) noexcept
{
    return ::get_config(mtype, device_config);
}

void device_config_to_json(rapidjson::Document& doc, Device const& dev) noexcept;
std::string device_config_to_json(Device const& dev) noexcept;

void device_route_to_json(rapidjson::Document& doc, Device const& dev) noexcept;
std::string device_route_to_json(Device const& dev) noexcept;

void device_full_config_to_json(rapidjson::Document& doc, Device const& dev) noexcept;
std::string device_full_config_to_json(Device const& dev) noexcept;

void device_board_to_json(rapidjson::Document& doc, Device const& dev) noexcept;
std::string device_board_to_json(Device const& dev) noexcept;

void device_uptime_to_json(rapidjson::Document& doc, Device const& dev) noexcept;
std::string device_uptime_to_json(Device const& dev) noexcept;

void device_rtc_time_to_json(rapidjson::Document& doc, Device const& dev) noexcept;
std::string device_rtc_time_to_json(Device const& dev) noexcept;

void device_fuse_to_json(rapidjson::Document& doc, Device const& dev) noexcept;
std::string device_fuse_to_json(Device const& dev) noexcept;

void device_jobs_to_json(rapidjson::Document& doc, Device const& dev) noexcept;
std::string device_jobs_to_json(Device const& dev) noexcept;

void device_ota_to_json(rapidjson::Document& doc, Device const& dev, std::string const& version) noexcept;
std::string device_ota_to_json(Device const& dev, std::string const& version) noexcept;

void device_apps_to_json(rapidjson::Document& doc, Device const& dev) noexcept;
std::string device_apps_to_json(Device const& dev) noexcept;

void device_to_json(rapidjson::Document& doc, Device const& dev) noexcept;
std::string device_to_json(Device const& dev) noexcept;

void device_list_to_json(rapidjson::Document& doc, Device_List const& list) noexcept;
std::string device_list_to_json(Device_List const& list) noexcept;

std::string device_edited_to_json(Device const& dev) noexcept;
std::string device_reset_reason_to_json(Device const&, long reason) noexcept;
std::string device_tree_to_json(Tree const& tree) noexcept;

std::string device_sensor_data(Device const& dev,
						Sensor::Sensor_List const& list,
						Sensor::Sensor_Type_List const& type_list) noexcept;
std::string device_sensor_data(Device const& dev,
						const void* sensor_data, std::size_t size,
						Sensor::Sensor_Type_List const& type_list) noexcept;
std::string device_sensor_data(Device const& dev,
						Sensor::sensor_type const&,
						Sensor::Sensor_Type_List const& type_list) noexcept;
std::string device_list_sensor_data(Device_List const& dev,
									Sensor::Sensor_Type_List const& type_list) noexcept;

std::string device_custom_response(
		CoAP::Message::message const& request,
		CoAP::Message::message const& response,
		endpoint const& ep,
		CoAP::Transmission::status_t status) noexcept;

}//Message
}//Device
}//Agro

#endif /* AGRO_DAEMON_MAKE_DEVICE_MESSAGE_HPP__ */
