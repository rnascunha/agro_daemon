#ifndef AGRO_DAEMON_MESSAGE_INFO_HPP__
#define AGRO_DAEMON_MESSAGE_INFO_HPP__

#include "rapidjson/document.h"
#include "make.hpp"
#include "../device/types.hpp"
#include <string>

namespace Message{

/**
 * Info
 */
enum class info{
	error = 0,
	warning,
	success,
	info
};

enum class info_category{
	general = 0,
	image,
	app,
	device
};

void make_info(rapidjson::Document&, info, const char*, const char* = nullptr) noexcept;
void make_info(rapidjson::Document&, info, info_category, const char*, const char* = nullptr) noexcept;
void make_info(rapidjson::Document&, info, mesh_addr_t const& addr, const char*, const char* = nullptr) noexcept;
void make_info(rapidjson::Document&, info, CoAP::Message::Option::option const& addr, const char*, const char* = nullptr) noexcept;

std::string make_info(info, info_category, const char*, const char* = nullptr) noexcept;
std::string make_info(info, const char*, const char* = nullptr) noexcept;
std::string make_info(info, mesh_addr_t const&, const char*, const char* = nullptr) noexcept;
std::string make_info(info, CoAP::Message::Option::option const& addr, const char*, const char* = nullptr) noexcept;

}//Message

#endif /* AGRO_DAEMON_MESSAGE_INFO_HPP__ */
