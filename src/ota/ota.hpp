#ifndef AGRO_DAEMON_OTA_HPP__
#define AGRO_DAEMON_OTA_HPP__

#include <filesystem>

void init_ota() noexcept;
void delete_image(std::vector<std::string> const& list) noexcept;
std::filesystem::path const& ota_path() noexcept;

#endif /* AGRO_DAEMON_OTA_HPP__ */
