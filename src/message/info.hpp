#ifndef AGRO_DAEMON_MESSAGE_INFO_HPP__
#define AGRO_DAEMON_MESSAGE_INFO_HPP__

#include "rapidjson/document.h"
#include "types.hpp"
#include "make.hpp"
#include "../device/types.hpp"
#include <string>

namespace Message{

void add_info(rapidjson::Document& doc, info inf) noexcept;

void make_info(rapidjson::Document& doc, info inf, const char* message) noexcept;
void make_info(rapidjson::Document& doc, info inf, mesh_addr_t const& addr, const char* message) noexcept;
void make_info(rapidjson::Document& doc, info inf, CoAP::Message::Option::option const& addr, const char* message) noexcept;

std::string make_info(info inf, const char* message) noexcept;
std::string make_info(info inf, mesh_addr_t const& addr, const char* message) noexcept;
std::string make_info(info inf, CoAP::Message::Option::option const& addr, const char* message) noexcept;

}//Message

#endif /* AGRO_DAEMON_MESSAGE_INFO_HPP__ */
