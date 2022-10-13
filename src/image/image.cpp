#include <system_error>
#include <filesystem>
#include <cstring>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>

#include "tt/tt.hpp"

#include "image.hpp"
#include "../helper/sha256.hpp"

#define HASH_LEN 32

namespace Agro{

Image_Path::Image_Path(std::filesystem::path const& path)
	: path_(path)
{
	if(std::filesystem::exists(path_) &&
		std::filesystem::is_directory(path_))
	{
		tt::debug("Images directory already exists");
		return;
	}

	std::error_code ec;
	std::filesystem::create_directory(path_, ec);
	if(ec)
	{
		tt::error("Error creating images directory [%d/%s]",
				ec.value(), ec.message().c_str());
		return;
	}

	tt::status("Created images '%s' directory", path_.c_str());
}

std::filesystem::path const& Image_Path::path() const noexcept
{
	return path_;
}

bool Image_Path::has(std::string const& image_file) const noexcept
{
	std::filesystem::path pi = make_path(image_file);

	return std::filesystem::exists(pi) && std::filesystem::is_regular_file(pi);
}

void Image_Path::erase(std::string const& image) const noexcept
{
	std::filesystem::remove(make_path(image));
}

void Image_Path::erase(std::vector<std::string> const& list) const noexcept
{
	for(auto const& s : list)
	{
		erase(s);
	}
}

std::filesystem::path Image_Path::make_path(std::string const& image) const noexcept
{
	std::stringstream ss;

	ss << path_.string() << "/" << image;

	return ss.str();
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

}//Agro
