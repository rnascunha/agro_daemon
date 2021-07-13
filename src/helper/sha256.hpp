#ifndef AGRO_DAEMON_HELPER_SHA256_HPP__
#define AGRO_DAEMON_HELPER_SHA256_HPP__

#include <cstdint>
#include <filesystem>
#include <openssl/sha.h>

using sha256_hash = std::uint8_t[SHA256_DIGEST_LENGTH];

bool sha256_file(std::filesystem::path const&, sha256_hash) noexcept;
std::size_t sha256_file(std::filesystem::path const& path,
		std::size_t max_bytes,
		sha256_hash hash) noexcept;
std::string sha256_string(sha256_hash const data) noexcept;

#endif /* AGRO_DAEMON_HELPER_SHA256_HPP__ */
