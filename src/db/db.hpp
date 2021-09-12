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

		bool read_users_infos(User::Info_List&) noexcept;
		bool read_users_sessions(User::Session_List&) noexcept;
		bool read_users_subscriptions(User::Subscription_List&) noexcept;
		bool read_user_all_db(User::Users&) noexcept;

		User::Info get_user(User::user_id id) noexcept;
		User::Info get_user(std::string const& username) noexcept;

		bool get_password(std::string const& username,
				std::vector<unsigned char>& salt,
				std::vector<unsigned char>& pass) noexcept;

		int add_user(std::string const& username,
						std::string const& password,
						User::Info::status status = User::Info::status::active,
						std::string const& name = std::string{},
						std::string const& email = std::string{}) noexcept;

		bool update_user_session_id(User::user_id,
				std::string const&,
				std::string const& user_agent) noexcept;

		bool check_user_session_id(User::user_id,
				std::string const& session_id,
				std::string const& user_agent,
				long& session_time) noexcept;

		bool clear_all_sessions() noexcept;
		bool clear_session(User::user_id) noexcept;
		bool clear_session_user_agent(User::user_id,
				std::string const& user_agent) noexcept;

		bool push_subscribe_user(User::user_id,
				std::string const& user_agent,
				std::string const& endpoint,
				std::string const& p256dh,
				std::string const& auth) noexcept;

		bool push_unsubscribe_user(User::user_id,
				std::string const& user_agent) noexcept;

		std::string notify_private_key() noexcept;

		const char* error(){ return db_.error(); };
	private:
		sqlite3 db_;
};

}//Agro

#endif /* AGRO_DAEMON_DB_HPP__ */
