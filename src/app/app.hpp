#ifndef AGRO_DAEMON_APP_HPP__
#define AGRO_DAEMON_APP_HPP__

#include <vector>
#include <string>
#include <filesystem>
#include "../helper/sha256.hpp"

void init_app() noexcept;
void delete_app(std::vector<std::string> const& list) noexcept;
std::filesystem::path const& app_path() noexcept;

bool calculate_app_hash(std::string const& path, sha256_hash hash) noexcept;
bool calculate_app_hash(std::filesystem::path const& path, sha256_hash hash) noexcept;

#endif /* AGRO_DAEMON_APP_HPP__ */
