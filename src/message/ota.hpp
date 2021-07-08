#ifndef AGRO_DAEMON_MESSAGE_OTA_HPP__
#define AGRO_DAEMON_MESSAGE_OTA_HPP__

#include "../ota/types.hpp"
#include "rapidjson/document.h"
#include <filesystem>

namespace Message {

template<typename Allocator>
rapidjson::Value& make_image(std::string const&,
		std::size_t,
		esp_app_desc_t const&,
		rapidjson::Value&, Allocator&) noexcept;

template<typename Allocator>
rapidjson::Value& make_image_list(std::filesystem::path const&,
		rapidjson::Value&,
		Allocator&) noexcept;

void ota_image_list(rapidjson::Document&, std::filesystem::path const&) noexcept;
std::string ota_image_list(std::filesystem::path const&) noexcept;

void process_image(rapidjson::Document const&) noexcept;

}//Message

#include "impl/ota_impl.hpp"

#endif /* AGRO_DAEMON_MESSAGE_OTA_HPP__ */
