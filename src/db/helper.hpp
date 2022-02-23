#ifndef AGRO_DAEMON_DB_HELPER_HPP__
#define AGRO_DAEMON_DB_HELPER_HPP__

#include <vector>

#include "../user/info.hpp"
#include "../user/group.hpp"
#include "../user/password.hpp"
#include "../libs/sqlite3/sqlite3.hpp"

namespace Agro{
namespace DB_Helper{

User::Info get_user(xeds::sqlite3&, std::string const& username) noexcept;

int add_user(xeds::sqlite3&,
		std::string const& username,
		User::key_password const password,
		User::salt_password const salt,
		std::string const& name,
		std::string const& email,
		std::string const& telegram_chat_id,
		User::user_id& id,
		User::Info::status status = User::Info::status::active) noexcept;

int read_user_groups(xeds::sqlite3&, Agro::User::Groups&) noexcept;

int add_user_to_group(xeds::sqlite3&, Agro::User::user_id, Agro::User::group_id) noexcept;
void add_user_to_groups(xeds::sqlite3&,
		Agro::User::user_id, std::vector<Agro::User::group_id> const&) noexcept;

}//DB_Helper
}//Agro

#endif /* AGRO_DAEMON_DB_HELPER_HPP__ */
