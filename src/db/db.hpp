#ifndef AGRO_DAEMON_DB_HPP__
#define AGRO_DAEMON_DB_HPP__

#include "sqlite3.hpp"
#include <system_error>
#include <string>
#include <vector>

#include "../user/user.hpp"

namespace Agro{

class DB{
	public:
		DB(std::string const& db_name, std::error_code& ec);

		User get_user(int id) noexcept;
		User get_user(std::string const& username) noexcept;
		User get_user(std::string const& username,
				std::vector<unsigned char>& salt,
				std::vector<unsigned char>& pass) noexcept;

		int add_user(std::string const& username,
						std::string const& password,
						User::status status = User::status::active,
						std::string const& name = std::string{},
						std::string const& email = std::string{}) noexcept;

		bool update_user_session_id(User const&,
				std::string const&,
				std::string const& user_agent) noexcept;

		bool check_user_session_id(User const&,
				std::string const& session_id,
				std::string const& user_agent,
				long& session_time) noexcept;

		bool clear_all_sessions() noexcept;
		bool clear_session(User const&) noexcept;
		bool clear_session_user_agent(User const&) noexcept;

		bool push_subscribe_user(User const&,
				std::string const& endpoint,
				std::string const& p256dh,
				std::string const& auth) noexcept;

		bool push_unsubscribe_user(User const&) noexcept;

		const char* error(){ return db_.error(); };
	private:
		sqlite3 db_;
};

}//Agro

#endif /* AGRO_DAEMON_DB_HPP__ */
