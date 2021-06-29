#ifndef AGRO_DAEMON_MAKE_DEVICE_MESSAGE_HPP__
#define AGRO_DAEMON_MAKE_DEVICE_MESSAGE_HPP__

#include "rapidjson/document.h"
#include "../device/device.hpp"
#include <string>

namespace Message{

void device_to_json(rapidjson::Document& doc, Device const&) noexcept;
std::string device_to_json(Device const&) noexcept;

}

#endif /* AGRO_DAEMON_MAKE_DEVICE_MESSAGE_HPP__ */
