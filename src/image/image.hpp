#ifndef AGRO_DAEMON_IMAGE_HPP__
#define AGRO_DAEMON_IMAGE_HPP__

#include <vector>
#include <string>
#include <filesystem>

#include "../user/user.hpp"

#include "esp_types.hpp"
#include "../helper/sha256.hpp"

namespace Agro{

class Image_Path{
	public:
		Image_Path(std::filesystem::path const&);

		std::filesystem::path const& path() const noexcept;

		bool has(std::string const&) const noexcept;

		void erase(std::string const&) const noexcept;
		void erase(std::vector<std::string> const&) const noexcept;

		std::filesystem::path make_path(std::string const&) const noexcept;
	private:
		std::filesystem::path path_;
};

struct Image{
	int id;
	std::string name;
	User::user_id uploader;
	std::string description;
	unsigned int time;
};

bool get_image_hash(std::filesystem::path const& path, sha256_hash hash) noexcept;
bool get_image_description(std::filesystem::path const& path, esp_app_desc_t& image_description) noexcept;
bool check_image(std::filesystem::path const& path) noexcept;

}//Agro

#endif /* AGRO_DAEMON_IMAGE_HPP__ */
