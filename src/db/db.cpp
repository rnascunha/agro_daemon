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

bool DB::read_users_infos(User::Info_List& infos) noexcept
{
	sqlite3::statement res;
	if(db_.prepare("SELECT userid,username,name,status,email FROM user",
			res) != SQLITE_OK)
	{
		return false;
	}

	while(res.step() == SQLITE_ROW)
	{
		infos.add(User::Info{res.interger(0),
				res.text(1),
				res.text(2),
				static_cast<User::Info::status>(res.interger(3)),
				res.text(4)});
	}

	return true;
}

bool DB::read_users_sessions(User::Session_List& sessions) noexcept
{
	sqlite3::statement res;
	if(db_.prepare("SELECT userid,user_agent,session_id,session_time FROM session",
			res) != SQLITE_OK)
	{
		return false;
	}

	while(res.step() == SQLITE_ROW)
	{
		sessions.add_or_update(User::Session{
				res.interger(0),
				res.text(1),
				res.text(2),
				db_time_to_epoch(res.text(3))});
	}
	return true;
}

bool DB::read_users_subscriptions(User::Subscription_List& subscriptions) noexcept
{
	sqlite3::statement res;
	if(db_.prepare("SELECT userid,user_agent,endpoint,p256dh,auth FROM push_notify",
			res) != SQLITE_OK)
	{
		return false;
	}

	while(res.step() == SQLITE_ROW)
	{
		subscriptions.add_or_update(
				res.interger(0),
				res.text(1),
				res.text(2),
				res.text(3),
				res.text(4));
	}
	return true;
}

bool DB::read_users_groups(User::Groups& groups) noexcept
{
	/**
	 * Adding groups
	 */
	sqlite3::statement res;
	if(db_.prepare("SELECT user_groupid,name,description FROM user_group",
			res) != SQLITE_OK)
	{
		return false;
	}

	while(res.step() == SQLITE_ROW)
	{
		groups.add(User::Group{
				res.interger(0),
				res.text(1),
				res.text(2)});
	}

	/**
	 * Adding users to groups
	 */
	res.reset();
	if(db_.prepare("SELECT userid, user_groupid FROM user_user_group", res) != SQLITE_OK)
	{
		return false;
	}

	while(res.step() == SQLITE_ROW)
	{
		groups.add_user(res.interger(1), res.interger(0));
	}

	return true;
}

bool DB::read_user_all_db(User::Users& users) noexcept
{
	if(!read_users_infos(users.infos())) return false;
	if(!read_users_sessions(users.sessions())) return false;
	if(!read_users_subscriptions(users.subscriptions())) return false;
	if(!read_users_groups(users.groups())) return false;

	return true;
}

User::Info DB::get_user(User::user_id id) noexcept
{
	sqlite3::statement res;
	if(db_.prepare_bind("SELECT username,name,status,email FROM user WHERE userid = ?",
			res, id) != SQLITE_OK)
	{
		return User::Info{};
	}

	if(res.step() != SQLITE_ROW)
	{
		return User::Info{};
	}

	return User::Info{id,
		res.text(0),
		res.text(2),
		static_cast<User::Info::status>(res.interger(3)),
		res.text(4)};
}

User::Info DB::get_user(std::string const& username) noexcept
{
	sqlite3::statement res;
	if(db_.prepare_bind("SELECT userid,name,status,email FROM user WHERE username = ?",
			res, username) != SQLITE_OK)
	{
		return User::Info{};
	}

	if(res.step() != SQLITE_ROW)
	{
		return User::Info{};
	}

	return User::Info{res.interger(0),
		username,
		res.text(1),
		static_cast<User::Info::status>(res.interger(2)),
		res.text(3)};
}

bool DB::get_password(std::string const& username,
		std::vector<unsigned char>& salt,
		std::vector<unsigned char>& pass) noexcept
{
	sqlite3::statement res;
	if(db_.prepare_bind("SELECT userid,password,name,status,email,salt FROM user WHERE username = ?",
			res, username) != SQLITE_OK)
	{
		return false;
	}

	if(res.step() != SQLITE_ROW)
	{
		return false;
	}

	pass = res.blob(1);
	salt = res.blob(5);

	return true;
}

int DB::add_user(std::string const& username,
		std::string const& password,
		User::Info::status status /* = User::status::active */,
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

Agro::User::group_id DB::add_user_group(std::string const& name,
				std::string const& description /* = std::string{} */) noexcept
{
	sqlite3::statement res;
	if(db_.prepare_bind("INSERT INTO user_group(name, description) VALUES(?,?)",
			res, name, description) != SQLITE_OK)
	{
		return -1;
	}
	res.step();

	return db_.last_insert_rowid();
}

int DB::add_user_to_group(Agro::User::group_id gid, Agro::User::user_id uid) noexcept
{
	sqlite3::statement res;
	int rc = db_.prepare_bind(
			"INSERT INTO user_user_group(user_groupid, userid) VALUES(?,?)",
			res, gid, uid);
	if(rc != SQLITE_OK)
	{
		return rc;
	}

	return res.step();
}

bool DB::update_user_session_id(User::user_id id,
				std::string const& session_id,
				std::string const& user_agent) noexcept
{
	sqlite3::statement res;
	if(db_.prepare_bind("REPLACE INTO session(session_id, userid, user_agent) VALUES(?,?,?)",
			res, session_id, id, user_agent) != SQLITE_OK)
	{
		return false;
	}

	if(res.step() != SQLITE_DONE)
	{
		return false;
	}

	return true;
}

bool DB::check_user_session_id(User::user_id id,
				std::string const& session_id,
				std::string const& user_agent,
				long& session_time) noexcept
{
	session_time = 0;
	sqlite3::statement res;
	if(db_.prepare_bind("SELECT session_time "
			"FROM session "
			"WHERE userid = ? AND user_agent = ? AND session_id = ?",
			res, id, user_agent, session_id) != SQLITE_OK)
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

bool DB::clear_session(User::user_id id) noexcept
{
	sqlite3::statement res;
	if(db_.prepare_bind("DELETE FROM session WHERE userid = ?",
			res, id) != SQLITE_OK)
	{
		return false;
	}

	return res.step() == SQLITE_DONE;
}

bool DB::clear_session_user_agent(User::user_id id, std::string const& user_agent) noexcept
{
	sqlite3::statement res;
	if(db_.prepare_bind("DELETE FROM session WHERE userid = ? AND user_agent = ?",
			res, id, user_agent) != SQLITE_OK)
	{
		return false;
	}

	return res.step() == SQLITE_DONE;
}

bool DB::push_subscribe_user(User::user_id id,
		std::string const& user_agent,
		std::string const& endpoint,
		std::string const& p256dh,
		std::string const& auth) noexcept
{
	sqlite3::statement res;
	if(db_.prepare_bind("REPLACE INTO push_notify(userid, user_agent, endpoint, p256dh, auth) VALUES(?,?,?,?,?)",
			res, id, user_agent, endpoint, p256dh, auth) != SQLITE_OK)
	{
		return false;
	}

	return res.step() == SQLITE_DONE;
}

bool DB::push_unsubscribe_user(User::user_id id,
		std::string const& user_agent) noexcept
{
	sqlite3::statement res;
	if(db_.prepare_bind("DELETE FROM push_notify WHERE userid = ? AND user_agent = ?",
			res, id, user_agent) != SQLITE_OK)
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
