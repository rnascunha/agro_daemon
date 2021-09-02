#include "db.hpp"
#include <string>
#include <iostream>
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
	static constexpr const
	std::string_view stmt{"SELECT username,name,status,email FROM user WHERE userid = ?"};

	sqlite3::statement res;
	if(db_.prepare(stmt, res) != SQLITE_OK)
	{
		return User{};
	}

	res.bind(1, id);

	;
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
	static constexpr const
	std::string_view stmt{"SELECT userid,name,status,email FROM user WHERE username = ?"};

	sqlite3::statement res;
	if(db_.prepare(stmt, res) != SQLITE_OK)
	{
		return User{};
	}

	res.bind(1, username);

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
	static constexpr const
	std::string_view stmt{"SELECT userid,password,name,status,email,salt FROM user WHERE username = ?"};

	sqlite3::statement res;
	if(db_.prepare(stmt, res) != SQLITE_OK)
	{
		return User{};
	}

	res.bind(1, username);

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
	static constexpr const
		std::string_view stmt{"INSERT INTO user(username,password,name,status,email) VALUES(?,?,?,?,?)"};

	sqlite3::statement res;
	int rc = db_.prepare(stmt, res);
	if(rc != SQLITE_OK) return rc;

	res.bind(1, username);
	res.bind(2, password);
	res.bind(3, name);
	res.bind(4, static_cast<int>(status));
	res.bind(5, email);

	return res.step();
}


bool DB::update_user_session_id(User const& user,
				std::string const& session_id,
				std::string const& user_agent) noexcept
{
	static constexpr const
		std::string_view stmt{"REPLACE INTO session(session_id, userid, user_agent) VALUES(?,?,?)"};

	sqlite3::statement res;
	if(db_.prepare(stmt, res) != SQLITE_OK)
	{
		return false;
	}

	res.bind(1, session_id);
	res.bind(2, user.id());
	res.bind(3, user_agent);

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
	static constexpr const
		std::string_view stmt{"SELECT session_time "
							"FROM session "
							"WHERE userid = ? AND user_agent = ? AND session_id = ?"};
	session_time = 0;
	sqlite3::statement res;
	if(db_.prepare(stmt, res) != SQLITE_OK)
	{
		return false;
	}

	res.bind(1, user.id());
	res.bind(2, user_agent);
	res.bind(3, session_id);

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
	static constexpr const
			std::string_view stmt{"DELETE FROM session"};

	sqlite3::statement res;
	if(db_.prepare(stmt, res) != SQLITE_OK)
	{
		return false;
	}

	return res.step() == SQLITE_DONE;
}

bool DB::clear_session(User const& user) noexcept
{
	static constexpr const
			std::string_view stmt{"DELETE FROM session WHERE userid = ?"};

	sqlite3::statement res;
	if(db_.prepare(stmt, res) != SQLITE_OK)
	{
		return false;
	}

	res.bind(1, user.id());

	return res.step() == SQLITE_DONE;
}

bool DB::clear_session_user_agent(User const& user) noexcept
{
	static constexpr const
			std::string_view stmt{"DELETE FROM session WHERE userid = ? AND user_agent = ?"};

	sqlite3::statement res;
	if(db_.prepare(stmt, res) != SQLITE_OK)
	{
		return false;
	}

	res.bind(1, user.id());
	res.bind(2, user.user_agent());

	return res.step() == SQLITE_DONE;
}

}//Agro
