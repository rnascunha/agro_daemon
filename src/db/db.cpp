#include "db.hpp"
#include <string>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

//https://stackoverflow.com/a/16166149
#ifdef _WIN32
#define timegm		_mkgmtime
#endif /* _WIN32 */

namespace Agro{

static long db_time_to_epoch(std::string const& str_time)
{
	std::tm tm;
	std::stringstream ss{str_time};
	ss >> std::get_time(&tm, "%Y-%m-%d  %H:%M:%S");
	return std::chrono::duration_cast<std::chrono::seconds>(
			std::chrono::system_clock::from_time_t(
					timegm(&tm)
						).time_since_epoch())
							.count();
}

DB::DB(std::string const& db_name, std::error_code& ec)
	: db_(db_name.c_str(), ec)
{
	if(ec) return;
	clear_all_sessions();
}

User DB::get_user(int id) noexcept
{
	sqlite3::statement res;
	if(db_.prepare_bind("SELECT username,name,status,email FROM user WHERE userid = ?",
			res, id) != SQLITE_OK)
	{
		return User{};
	}

	if(res.step() != SQLITE_ROW)
	{
		return User{};
	}

	return User{id,
		res.text(0),
		res.text(2),
		static_cast<User::status>(res.interger(3)),
		res.text(4)};
}

User DB::get_user(std::string const& username) noexcept
{
	sqlite3::statement res;
	if(db_.prepare_bind("SELECT userid,name,status,email FROM user WHERE username = ?",
			res, username) != SQLITE_OK)
	{
		return User{};
	}

	if(res.step() != SQLITE_ROW)
	{
		return User{};
	}

	return User{res.interger(0),
		username,
		res.text(1),
		static_cast<User::status>(res.interger(2)),
		res.text(3)};
}

User DB::get_user(std::string const& username,
		std::vector<unsigned char>& salt,
		std::vector<unsigned char>& pass) noexcept
{
	sqlite3::statement res;
	if(db_.prepare_bind("SELECT userid,password,name,status,email,salt FROM user WHERE username = ?",
			res, username) != SQLITE_OK)
	{
		return User{};
	}

	if(res.step() != SQLITE_ROW)
	{
		return User{};
	}

	pass = res.blob(1);
	salt = res.blob(5);

	return User{res.interger(0),
		username.c_str(),
		res.text(2),
		static_cast<User::status>(res.interger(3)),
		res.text(4)};
}


int DB::add_user(std::string const& username,
		std::string const& password,
		User::status status /* = User::status::active */,
		std::string const& name /* = std::string{} */,
		std::string const& email /* = std::string{} */) noexcept
{
	sqlite3::statement res;
	int rc = db_.prepare_bind("INSERT INTO user(username,password,name,status,email) VALUES(?,?,?,?,?)",
			res, username, password, name, static_cast<int>(status), email);
	if(rc != SQLITE_OK)
	{
		return rc;
	}

	return res.step();
}


bool DB::update_user_session_id(User const& user,
				std::string const& session_id,
				std::string const& user_agent) noexcept
{
	sqlite3::statement res;
	if(db_.prepare_bind("REPLACE INTO session(session_id, userid, user_agent) VALUES(?,?,?)",
			res, session_id, user.id(), user_agent) != SQLITE_OK)
	{
		return false;
	}

	if(res.step() != SQLITE_DONE)
	{
		return false;
	}

	return true;
}

bool DB::check_user_session_id(User const& user,
				std::string const& session_id,
				std::string const& user_agent,
				long& session_time) noexcept
{
	session_time = 0;
	sqlite3::statement res;
	if(db_.prepare_bind("SELECT session_time "
			"FROM session "
			"WHERE userid = ? AND user_agent = ? AND session_id = ?",
			res, user.id(), user_agent, session_id) != SQLITE_OK)
	{
		return false;
	}

	if(res.step() != SQLITE_ROW)
	{
		return false;
	}

	std::string const str_time = res.text(0);
	session_time = db_time_to_epoch(str_time);

	return true;
}

bool DB::clear_all_sessions() noexcept
{
	sqlite3::statement res;
	if(db_.prepare("DELETE FROM session", res) != SQLITE_OK)
	{
		return false;
	}

	return res.step() == SQLITE_DONE;
}

bool DB::clear_session(User const& user) noexcept
{
	sqlite3::statement res;
	if(db_.prepare_bind("DELETE FROM session WHERE userid = ?",
			res, user.id()) != SQLITE_OK)
	{
		return false;
	}

	return res.step() == SQLITE_DONE;
}

bool DB::clear_session_user_agent(User const& user) noexcept
{
	sqlite3::statement res;
	if(db_.prepare_bind("DELETE FROM session WHERE userid = ? AND user_agent = ?",
			res, user.id(), user.user_agent()) != SQLITE_OK)
	{
		return false;
	}

	return res.step() == SQLITE_DONE;
}

bool DB::push_subscribe_user(User const& user,
		std::string const& endpoint,
		std::string const& p256dh,
		std::string const& auth) noexcept
{
	sqlite3::statement res;
	if(db_.prepare_bind("REPLACE INTO push_notify(userid, user_agent, endpoint, p256dh, auth) VALUES(?,?,?,?,?)",
			res, user.id(), user.user_agent(), endpoint, p256dh, auth) != SQLITE_OK)
	{
		return false;
	}

	return res.step() == SQLITE_DONE;
}

bool DB::push_unsubscribe_user(User const& user) noexcept
{
	sqlite3::statement res;
	if(db_.prepare_bind("DELETE FROM push_notify WHERE userid = ? AND user_agent = ?",
			res, user.id(), user.user_agent()) != SQLITE_OK)
	{
		return false;
	}

	return res.step() == SQLITE_DONE;
}

std::string DB::notify_private_key() noexcept
{
	sqlite3::statement res;
	if(db_.prepare("SELECT notify_private_key FROM instance LIMIT 1", res) != SQLITE_OK)
	{
		return std::string{};
	}

	if(res.step() != SQLITE_ROW) return std::string{};

	return res.text(0);
}

}//Agro
