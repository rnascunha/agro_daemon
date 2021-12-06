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

std::string DB::notify_telegram_bot_token() noexcept
{
	sqlite3::statement res;
	if(db_.prepare("SELECT telegram_bot_token FROM instance LIMIT 1", res) != SQLITE_OK)
	{
		return std::string{};
	}

	if(res.step() != SQLITE_ROW) return std::string{};

	return res.text(0);
}

}//Agro
