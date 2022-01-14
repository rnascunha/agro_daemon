#include "db.hpp"

namespace Agro{

int DB::read_general_notify(User::user_id id, Notify::Notify& notify) noexcept
{
	sqlite3::statement res;
	int rc = db_.prepare_bind("SELECT notify FROM notify_general WHERE userid = ?",
			res, id);
	if(rc != SQLITE_OK)
	{
		return rc;
	}

	rc = res.step();
	if(rc != SQLITE_ROW)
	{
		return rc;
	}

	notify.set(static_cast<Notify::general_type>(res.integer(0)));

	return rc;
}

int DB::update_general_notify(User::user_id id, Notify::general_type type) noexcept
{
	sqlite3::statement res;
	int rc = db_.prepare_bind("REPLACE INTO notify_general(userid,notify) VALUES(?,?)",
			res, id, static_cast<int>(type));
	if(rc != SQLITE_OK)
	{
		return rc;
	}

	return res.step();
}

int DB::read_device_notify(User::user_id id, Notify::Notify& notify) noexcept
{
	sqlite3::statement res;
	int rc = db_.prepare_bind("SELECT deviceid,notify FROM notify_device WHERE userid = ?",
			res, id);
	if(rc != SQLITE_OK)
	{
		return rc;
	}

	do
	{
		rc = res.step();
		if(rc != SQLITE_ROW) break;

		notify.set(res.integer(0), static_cast<Notify::device_type>(res.integer(1)));
	}while(true);

	return rc;

}

int DB::update_device_notify(User::user_id id, Device::device_id dev_id, Notify::device_type type) noexcept
{
	sqlite3::statement res;
	int rc = db_.prepare_bind("REPLACE INTO notify_device(userid,deviceid,notify) VALUES(?,?,?)",
			res, id, dev_id, static_cast<int>(type));
	if(rc != SQLITE_OK)
	{
		return rc;
	}

	return res.step();
}

int DB::read_sensor_notify(User::user_id id, Notify::Notify& notify) noexcept
{
	sqlite3::statement res;
	int rc = db_.prepare_bind("SELECT deviceid,sensor_type,sensor_index,notify_type,enabled,value "
									"FROM notify_sensor WHERE userid = ?",
			res, id);
	if(rc != SQLITE_OK)
	{
		return rc;
	}

	do
	{
		rc = res.step();
		if(rc != SQLITE_ROW) break;

		notify.add(res.integer(0), res.integer(1), res.integer(2),
					res.integer(4), static_cast<Notify::sensor_type>(res.integer(3)),
					res.real(5));
	}while(true);

	return rc;
}

int DB::update_sensor_notify(User::user_id uid, Device::device_id did,
		std::vector<Notify::sensor_temp_notify> const& sensors) noexcept
{
	sqlite3::statement res;
	int rc = db_.prepare_bind("DELETE FROM notify_sensor WHERE deviceid = ? AND userid = ?",
			res, did, uid);
	if(rc != SQLITE_OK)
	{
		return rc;
	}

	rc = res.step();
	if(rc != SQLITE_DONE)
	{
		return rc;
	}
	res.finalize();

	rc = db_.prepare("INSERT INTO notify_sensor(userid,deviceid,sensor_type,sensor_index,notify_type,enabled,value) "
					"VALUES(?,?,?,?,?,?,?)", res);
	if(rc != SQLITE_OK)
	{
		return rc;
	}

	for(auto const& s : sensors)
	{
		res.reset();
		res.clear_bidings();

		res.bind_all(uid, did, s.type, s.index, static_cast<int>(s.stype), s.enabled, s.value);

		rc = res.step();
		if(rc != SQLITE_DONE) break;
	}

	return rc;
}

std::string DB::notify_private_key(bool& enable) noexcept
{
	enable = false;

	sqlite3::statement res;
	if(db_.prepare("SELECT notify_private_key,push_enable FROM instance LIMIT 1", res) != SQLITE_OK)
	{
		return std::string{};
	}

	if(res.step() != SQLITE_ROW) return std::string{};
	enable = res.integer(1);

	return res.text(0);
}

int DB::update_push_notify(bool enable) noexcept
{
	sqlite3::statement res;
	int rc = db_.prepare_bind("UPDATE instance SET push_enable = ?",
			res, enable);
	if(rc != SQLITE_OK)
	{
		return rc;
	}

	return res.step();
}

std::string DB::notify_telegram_bot_token(bool& enable) noexcept
{
	enable = false;

	sqlite3::statement res;
	if(db_.prepare("SELECT telegram_bot_token,telegram_enable FROM instance LIMIT 1", res) != SQLITE_OK)
	{
		return std::string{};
	}

	if(res.step() != SQLITE_ROW) return std::string{};
	enable = res.integer(1);

	return res.text(0);
}

int DB::update_telegram_bot_token(std::string const& token, bool enable) noexcept
{
	sqlite3::statement res;
	int rc = db_.prepare_bind("UPDATE instance SET "
			"telegram_bot_token = ?, telegram_enable = ?",
			res, token, enable);
	if(rc != SQLITE_OK)
	{
		return rc;
	}

	return res.step();
}

int DB::notify_mail_server_info(SMTP::server& server, bool& enable) noexcept
{
	enable = false;

	sqlite3::statement res;
	int rc = db_.prepare("SELECT smtp_server,smtp_port,smtp_user,smtp_password,mail_enable FROM instance LIMIT 1", res);
	if(rc != SQLITE_OK)
	{
		return rc;
	}

	rc = res.step();
	if(rc != SQLITE_ROW)
	{
		return rc;
	}

	server.server = res.text(0);
	server.port = res.text(1);
	server.user = res.text(2);
	server.password = res.text(3);
	enable = res.integer(4);

	return rc;
}

int DB::update_mail_server_info(SMTP::server const& server, bool enable) noexcept
{
	sqlite3::statement res;
	int rc = db_.prepare_bind("UPDATE instance SET "
				"smtp_server = ?, smtp_port = ?, smtp_user = ?, smtp_password = ?, mail_enable = ?",
			res, server.server, server.port, server.user, server.password, enable);
	if(rc != SQLITE_OK)
	{
		return rc;
	}

	return res.step();
}

}//Agro
