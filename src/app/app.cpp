#include "app.hpp"
#include <sstream>

#include "tt/tt.hpp"

namespace Agro{

App_Path::App_Path(std::filesystem::path const& path)
	: path_(path)
{
	if(std::filesystem::exists(path_) &&
		std::filesystem::is_directory(path_))
	{
		tt::debug("App directory already exists");
		return;
	}

	std::error_code ec;
	std::filesystem::create_directory(path_, ec);
	if(ec)
	{
		tt::error("Error creating apps directory [%d/%s]",
				ec.value(), ec.message().c_str());
		return;
	}

	tt::status("Created apps '%s' directory", path_.c_str());
}

std::filesystem::path const& App_Path::path() const noexcept
{
	return path_;
}

void App_Path::erase(std::string const& image) const noexcept
{
	std::filesystem::remove(make_path(image));
}

void App_Path::erase(std::vector<std::string> const& list) const noexcept
{
	for(auto const& s : list)
	{
		erase(s);
	}
}

std::filesystem::path App_Path::make_path(std::string const& image) const noexcept
{
	std::stringstream ss;

	ss << path_.string() << "/" << image;

	return ss.str();
}

bool calculate_app_hash(App_Path const& app_path,
		std::string const& path,
		sha256_hash hash) noexcept
{
	return calculate_app_hash(app_path.make_path(path), hash);
}

bool calculate_app_hash(std::filesystem::path const& path, sha256_hash hash) noexcept
{
	if(!std::filesystem::exists(path) ||
		!std::filesystem::is_regular_file(path))
		return false;

	sha256_file(path, hash);

	return true;
}

}//Agro
