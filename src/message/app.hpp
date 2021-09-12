#ifndef AGRO_DAEMON_MESSAGE_APP_HPP__
#define AGRO_DAEMON_MESSAGE_APP_HPP__

#include "rapidjson/document.h"
#include <filesystem>
#include "../websocket/types.hpp"

namespace Message {

void app_list(rapidjson::Document& doc, std::filesystem::path const& path) noexcept;
std::string app_list(std::filesystem::path const& path) noexcept;

void process_app(rapidjson::Document const&, Agro::websocket_ptr) noexcept;

}//Message

#endif /* AGRO_DAEMON_MESSAGE_APP_HPP__ */