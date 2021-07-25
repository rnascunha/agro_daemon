#include "app.hpp"
#include <iostream>

static const std::filesystem::path apps_dir{"apps"};

void init_app() noexcept
{
	if(std::filesystem::exists(apps_dir) &&
		std::filesystem::is_directory(apps_dir))
	{
		std::cout << "Apps directory already exists\n";
		return;
	}

	std::error_code ec;
	std::filesystem::create_directory(apps_dir, ec);
	if(ec)
	{
		std::cerr << "Error creating apps directory: "
				<< ec.value() << "/" << ec.message() << "\n";
	}

	std::cout << "Created apps directory\n";
}

void delete_app(std::vector<std::string> const& list) noexcept
{
	for(auto const& s : list)
	{
		std::string p{apps_dir};
		p += "/";
		p += s;
		std::filesystem::remove(p);
	}
}

std::filesystem::path const& app_path() noexcept
{
	return apps_dir;
}

bool calculate_app_hash(std::filesystem::path const& path, sha256_hash hash) noexcept
{
	if(!std::filesystem::exists(path) ||
		!std::filesystem::is_regular_file(path))
		return false;

	sha256_file(path, hash);

	return true;
}
