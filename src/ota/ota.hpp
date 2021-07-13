#ifndef AGRO_DAEMON_OTA_HPP__
#define AGRO_DAEMON_OTA_HPP__

#include <vector>
#include <string>
#include <filesystem>

#include "types.hpp"
#include "../helper/sha256.hpp"

void init_ota() noexcept;
void delete_image(std::vector<std::string> const& list) noexcept;
std::filesystem::path const& ota_path() noexcept;

bool get_image_hash(std::filesystem::path const& path, sha256_hash hash) noexcept;
bool get_image_description(std::filesystem::path const& path, esp_app_desc_t& image_description) noexcept;
bool check_image(std::filesystem::path const& path) noexcept;

#endif /* AGRO_DAEMON_OTA_HPP__ */
