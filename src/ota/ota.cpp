#include <system_error>
#include <filesystem>
#include <cstring>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include "ota.hpp"
#include "types.hpp"
#include "../helper/sha256.hpp"

static const std::filesystem::path images_dir{"images"};
#define HASH_LEN 32

void init_ota() noexcept
{
	if(std::filesystem::exists(images_dir) &&
		std::filesystem::is_directory(images_dir))
	{
		std::cout << "Images directory already exists\n";
		return;
	}

	std::error_code ec;
	std::filesystem::create_directory(images_dir, ec);
	if(ec)
	{
		std::cerr << "Error creating images directory: "
				<< ec.value() << "/" << ec.message() << "\n";
	}

	std::cout << "Created images directory\n";
}

void delete_image(std::vector<std::string> const& list) noexcept
{
	for(auto const& s : list)
	{
		std::string p{images_dir};
		p += "/";
		p += s;
		std::filesystem::remove(p);
	}
}

std::filesystem::path const& ota_path() noexcept
{
	return images_dir;
}

bool get_image_description(std::filesystem::path const& path, esp_app_desc_t& image_description) noexcept
{
	if(!std::filesystem::is_regular_file(path)) return false;

	std::size_t size = std::filesystem::file_size(path);
	if(size < (sizeof(esp_image_header_t) + sizeof(esp_image_segment_header_t) + sizeof(esp_app_desc_t)))
		return false;

	std::ifstream ifs{path, std::ios::binary};
	if(!ifs)
	{
		return false;
	}

	ifs.seekg(sizeof(esp_image_header_t) + sizeof(esp_image_segment_header_t), ifs.beg);
	ifs.read((char*)&image_description, sizeof(esp_app_desc_t));
	ifs.close();

	return true;
}

bool get_image_hash(std::filesystem::path const& path, sha256_hash hash) noexcept
{
	std::ifstream ifs{path, std::ios::binary};
	if(!ifs)
	{
		return false;
	}

	ifs.seekg(-SHA256_DIGEST_LENGTH, std::ios::end);
	ifs.read((char*)hash, SHA256_DIGEST_LENGTH);
	ifs.close();

	return true;
}

bool check_image(std::filesystem::path const& path) noexcept
{
	sha256_hash hash1, hash2;
	if(!std::filesystem::exists(path) ||
		!std::filesystem::is_regular_file(path))
		return false;

	sha256_file(path, std::filesystem::file_size(path) - SHA256_DIGEST_LENGTH, hash1);
	get_image_hash(path, hash2);

	return std::memcmp(hash1, hash2, SHA256_DIGEST_LENGTH) == 0;
}

