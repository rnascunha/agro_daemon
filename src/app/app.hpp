#ifndef AGRO_DAEMON_APP_HPP__
#define AGRO_DAEMON_APP_HPP__

#include <vector>
#include <string>
#include <filesystem>

#include "../user/user.hpp"

#include "../helper/sha256.hpp"

namespace Agro{

class App_Path{
	public:
		App_Path(std::filesystem::path const&);

		std::filesystem::path const& path() const noexcept;

		void erase(std::string const&) const noexcept;
		void erase(std::vector<std::string> const&) const noexcept;

		std::filesystem::path make_path(std::string const&) const noexcept;
	private:
		std::filesystem::path path_;
};

struct App{
	int id;
	std::string name;
	User::user_id uploader;
	std::string description;
	unsigned int time;
	sha256_hash	hash;
};

bool calculate_app_hash(App_Path const& app_path,
		std::string const& path,
		sha256_hash hash) noexcept;
bool calculate_app_hash(std::filesystem::path const& path,
		sha256_hash hash) noexcept;

}//Agro

#endif /* AGRO_DAEMON_APP_HPP__ */
