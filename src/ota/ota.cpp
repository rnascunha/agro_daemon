#include <system_error>
#include <filesystem>
#include <cstring>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include "ota.hpp"
#include "types.hpp"

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
