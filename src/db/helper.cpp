#include "helper.hpp"

namespace Agro{
namespace DB_Helper{

using xeds::sqlite3;

User::Info get_user(sqlite3& db, std::string const& username) noexcept
{
	sqlite3::statement res;
	if(db.prepare_bind("SELECT userid,name,status,email,telegram_chat_id FROM user WHERE username = ?",
			res, username) != SQLITE_OK)
	{
		return User::Info{};
	}

	if(res.step() != SQLITE_ROW)
	{
		return User::Info{};
	}

	return User::Info{
		username,
		res.text(1),
		static_cast<User::Info::status>(res.integer(2)),
		res.text(3),
		res.text(4)};
}

int add_user(sqlite3& db,
		std::string const& username,
		User::key_password const password,
		User::salt_password const salt,
		std::string const& name,
		std::string const& email,
		std::string const& telegram_chat_id,
		User::user_id& id,
		User::Info::status status /* = User::Info::status::active */) noexcept
{
	id = User::invalid_id;
	sqlite3::statement res;
	int rc = db.prepare_bind(
			"INSERT INTO user(username,password,name,status,email,salt,telegram_chat_id) VALUES(?,?,?,?,?,?,?)",
			res,
			username,
			sqlite3::binary{password, USER_AUTH_KEY_LENGTH},
			name,
			static_cast<int>(status),
			email,
			sqlite3::binary{salt, USER_AUTH_SALT_LENGTH},
			telegram_chat_id);
	if(rc != SQLITE_OK)
	{
		return rc;
	}

	rc = res.step();
	if(rc == SQLITE_DONE)
	{
		id = static_cast<User::user_id>(db.last_insert_rowid());
	}
	return rc;
}

int read_user_groups(xeds::sqlite3& db, Agro::User::Groups& groups) noexcept
{
	/**
	 * Adding groups
	 */
	sqlite3::statement res;
	int rc = db.prepare("SELECT user_groupid,name,description FROM user_group",
			res);
	if(rc != SQLITE_OK)
	{
		return rc;
	}

	while(res.step() == SQLITE_ROW)
	{
		groups.add(User::Group{
				res.integer(0),
				res.text(1),
				res.text(2)});
	}

	return SQLITE_DONE;
}

int add_user_to_group(xeds::sqlite3& db, Agro::User::user_id uid, Agro::User::group_id gid) noexcept
{
	sqlite3::statement res;
	int rc = db.prepare_bind(
			"INSERT INTO user_user_group(user_groupid, userid) VALUES(?,?)",
			res, gid, uid);
	if(rc != SQLITE_OK)
	{
		return rc;
	}

	return res.step();
}

void add_user_to_groups(xeds::sqlite3& db,
		Agro::User::user_id uid,
		std::vector<Agro::User::group_id> const& gids) noexcept
{
	for(auto const& gid : gids)
	{
		add_user_to_group(db, uid, gid);
	}
}

}//DB_Helper
}//Agro
