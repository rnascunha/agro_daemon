#include "db.hpp"
#include <string>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <memory>
#include "helper.hpp"

//https://stackoverflow.com/a/16166149
#ifdef _WIN32
#define timegm		_mkgmtime
#endif /* _WIN32 */

namespace Agro{

using xeds::sqlite3;

static long db_time_to_epoch(std::string const& str_time)
{
	std::tm tm;
	std::stringstream ss{str_time};
	ss >> std::get_time(&tm, "%Y-%m-%d  %H:%M:%S");
	return static_cast<long>(std::chrono::duration_cast<std::chrono::seconds>(
			std::chrono::system_clock::from_time_t(
					timegm(&tm)
						).time_since_epoch())
							.count());
}

bool DB::read_users_groups(User::User_List& users) noexcept
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
		users.add_group(User::Group{
				res.integer(0),
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
		users.add_user_to_group(res.integer(0), res.integer(1));
	}

	return true;
}

bool DB::read_user_subscriptions(User::user_id id,
		User::Subscription_List& subscriptions) noexcept
{
	sqlite3::statement res;
	if(db_.prepare_bind("SELECT userid,user_agent,endpoint,p256dh,auth FROM push_notify WHERE userid = ?",
			res, id) != SQLITE_OK)
	{
		return false;
	}

	while(res.step() == SQLITE_ROW)
	{
		subscriptions.add_or_update(
				res.text(1),
				res.text(2),
				res.text(3),
				res.text(4));
	}
	return true;
}

bool DB::read_user_sessions(User::user_id id, User::Session_List& sessions) noexcept
{
	sqlite3::statement res;
	if(db_.prepare_bind("SELECT userid,user_agent,session_id,session_time FROM session WHERE userid = ?",
			res, id) != SQLITE_OK)
	{
		return false;
	}

	while(res.step() == SQLITE_ROW)
	{
		sessions.add_or_update(User::Session{
				res.text(1),
				res.text(2),
				db_time_to_epoch(res.text(3))});
	}
	return true;
}

bool DB::read_user_all_db(User::User_List& users) noexcept
{
	sqlite3::statement res;
	if(db_.prepare("SELECT userid,username,name,status,email,telegram_chat_id FROM user",
			res) != SQLITE_OK)
	{
		return false;
	}

	while(res.step() == SQLITE_ROW)
	{
		User::user_id id = res.integer(0);
		auto* nuser = users.add(User::User{id,
				User::Info{
				res.text(1),
				res.text(2),
				static_cast<User::Info::status>(res.integer(3)),
				res.text(4),
				res.text(5)}});
		read_user_sessions(id, nuser->sessions());
		read_user_subscriptions(id, nuser->subscriptions());
		read_general_notify(id, nuser->notify());
		read_device_notify(id, nuser->notify());
		read_sensor_notify(id, nuser->notify());
	}

	if(!read_policies(users.policies())) return false;
	if(!read_users_groups(users)) return false;

	return true;
}

bool DB::read_policy_types(Authorization::Policy_Types& policy_types) noexcept
{
	sqlite3::statement res;
	int rc = db_.prepare("SELECT policyid, name, code, description FROM policy_type", res);
	if(rc != SQLITE_OK)
	{
		return false;
	}

	while(res.step() == SQLITE_ROW)
	{
		policy_types.emplace_back(
				res.integer(0),
				static_cast<Authorization::rule>(res.integer(2)),
				res.text(1), res.text(3));
	}

	return true;
}

bool DB::read_policies(Authorization::Policies& policies) noexcept
{
	sqlite3::statement res;
	int rc = db_.prepare("SELECT policyid, groupid, rules FROM policy", res);
	if(rc != SQLITE_OK)
	{
		return false;
	}

	while(res.step() == SQLITE_ROW)
	{
		policies.add(res.integer(0),
				res.integer(1),
				static_cast<Authorization::rule>(res.integer(2)));
	}

	return true;
}

User::Info DB::get_user(User::user_id id) noexcept
{
	sqlite3::statement res;
	if(db_.prepare_bind("SELECT username,name,status,email,telegram_chat_id FROM user WHERE userid = ?",
			res, id) != SQLITE_OK)
	{
		return User::Info{};
	}

	if(res.step() != SQLITE_ROW)
	{
		return User::Info{};
	}

	return User::Info{
		res.text(0),
		res.text(2),
		static_cast<User::Info::status>(res.integer(3)),
		res.text(4),
		res.text(5)};
}

User::Info DB::get_user(std::string const& username) noexcept
{
	return DB_Helper::get_user(db_, username);
}

bool DB::get_root_password(std::vector<unsigned char>& salt,
					std::vector<unsigned char>& pass) noexcept
{
	sqlite3::statement res;
	if(db_.prepare("SELECT root_password,root_salt FROM instance LIMIT 1",
			res) != SQLITE_OK)
	{
		return false;
	}

	if(res.step() != SQLITE_ROW)
	{
		return false;
	}

	pass = res.blob(0);
	salt = res.blob(1);

	return true;
}

bool DB::get_password(std::string const& username,
		std::vector<unsigned char>& salt,
		std::vector<unsigned char>& pass) noexcept
{
	if(username == "root")
	{
		return get_root_password(salt, pass);
	}
	sqlite3::statement res;
	if(db_.prepare_bind("SELECT password,salt FROM user WHERE username = ?",
			res, username) != SQLITE_OK)
	{
		return false;
	}

	if(res.step() != SQLITE_ROW)
	{
		return false;
	}

	pass = res.blob(0);
	salt = res.blob(1);

	return true;
}

int DB::add_user(std::string const& username,
			User::key_password const password,
			User::salt_password const salt,
			std::string const& name,
			std::string const& email,
			std::string const& telegram_chat_id,
			User::user_id& id,
			User::Info::status status /* = User::Info::status::active */) noexcept
{
	return DB_Helper::add_user(db_, username, password, salt, name, email, telegram_chat_id, id, status);
}

int DB::edit_user(User::user_id id,
				std::string const& username,
				std::string const& name,
				std::string const& email,
				std::string const& telegram_chat_id,
				std::vector<User::group_id> const& groups) noexcept
{
	sqlite3::statement res;
	int rc = db_.prepare_bind(
			"UPDATE user SET username = ?, name = ?, email = ?, telegram_chat_id = ? WHERE userid = ?",
			res, username, name, email, telegram_chat_id, id);
	if(rc != SQLITE_OK)
	{
		return rc;
	}

	rc = res.step();
	if(rc != SQLITE_DONE)
	{
		return rc;
	}

	return set_user_to_groups(id, groups);
}

int DB::delete_user(User::user_id id) noexcept
{
	sqlite3::statement res;
	int rc = db_.prepare_bind("DELETE FROM user WHERE userid = ?", res, id);
	if(rc != SQLITE_OK)
	{
		return rc;
	}

	return res.step();
}

int DB::delete_user_from_group(User::user_id id) noexcept
{
	sqlite3::statement res;
	int rc = db_.prepare_bind("DELETE FROM user_user_group WHERE userid = ?", res, id);
	if(rc != SQLITE_OK)
	{
		return rc;
	}

	return res.step();
}

int DB::delete_group(User::group_id gid) noexcept
{
	sqlite3::statement res;
	int rc = db_.prepare_bind(
			"DELETE FROM user_user_group WHERE user_groupid = ?",
			res,
			static_cast<int>(gid));
	if(rc != SQLITE_OK)
	{
		return rc;
	}

	rc = res.step();
	if(rc != SQLITE_DONE)
	{
		return rc;
	}

	res.reset();
	rc = db_.prepare_bind(
				"DELETE FROM user_group WHERE user_groupid = ?",
				res,
				static_cast<int>(gid));

	if(rc != SQLITE_OK)
	{
		return rc;
	}

	return res.step();
}

int DB::remove_policy_group(User::group_id gid) noexcept
{
	sqlite3::statement res;
	int rc = db_.prepare_bind(
				"DELETE FROM policy WHERE groupid = ?",
				res, gid);
	if(rc != SQLITE_OK)
	{
		return rc;
	}

	return res.step();
}

int DB::add_user_group(std::string const& name,
				std::string const& description,
				User::group_id& id) noexcept
{
	id = User::invalid_id;
	sqlite3::statement res;
	int rc = db_.prepare_bind("INSERT INTO user_group(name, description) VALUES(?,?)",
			res, name, description);
	if(rc != SQLITE_OK)
	{
		return rc;
	}

	rc = res.step();
	if(rc == SQLITE_DONE)
	{
		id = static_cast<User::group_id>(db_.last_insert_rowid());
	}

	return rc;
}

int DB::add_user_to_group(Agro::User::group_id gid, Agro::User::user_id uid) noexcept
{
	return DB_Helper::add_user_to_group(db_, uid, gid);
}

void DB::add_users_to_group(User::group_id gid, std::vector<User::user_id> const& members) noexcept
{
	for(auto const& m : members)
	{
		add_user_to_group(gid, m);
	}
}

int DB::set_user_to_groups(User::user_id uid, std::vector<User::group_id> const& gid_list) noexcept
{
	int rc = remove_all_user_groups(uid);
	if(rc != SQLITE_DONE)
	{
		return rc;
	}

	DB_Helper::add_user_to_groups(db_, uid, gid_list);

	return SQLITE_DONE;
}

int DB::remove_all_user_groups(User::user_id uid) noexcept
{
	sqlite3::statement res;
	int rc = db_.prepare_bind("DELETE FROM user_user_group WHERE userid = ?", res, uid);
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

}//Agro
