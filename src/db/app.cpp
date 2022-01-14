#include "db.hpp"
#include "../helper/time_helper.hpp"
#include <iostream>

namespace Agro{

int DB::add_app(std::string const& app_name,
					User::user_id uploader_id,
					sha256_hash const& hash,
					std::string const& description /* = "" */) noexcept
{
	sqlite3::statement res;
	int rc = db_.prepare_bind("INSERT INTO app(name,uploader_id,description,date_upload, hash) VALUES(?,?,?,?,?)",
			res,
			app_name,
			uploader_id,
			description,
			time_epoch_seconds(),
			sqlite3::binary{hash, SHA256_DIGEST_LENGTH});

	if(rc != SQLITE_OK)
	{
		return rc;
	}

	return res.step();
}

int DB::read_app(std::string const& app_name,
		App& app) noexcept
{
	sqlite3::statement res;
	int rc = db_.prepare_bind("SELECT appid,uploader_id,description,date_upload,hash FROM app WHERE name = ?",
			res, app_name);
	if(rc != SQLITE_OK)
	{
		return rc;
	}

	rc = res.step();
	if(rc != SQLITE_ROW)
	{
		return rc;
	}

	app.id = res.integer(0);
	app.uploader = res.integer(1);
	app.description = res.text(2);
	app.time = res.integer(3);

	auto const hash = res.blob(4);
	std::memcpy(app.hash, hash.data(), hash.size());

	return rc;
}

int DB::update_app(std::string const& app_name,
		std::string const& description) noexcept
{
	sqlite3::statement res;
	int rc = db_.prepare_bind(
			"UPDATE app SET description = ? WHERE name = ?",
			res,
			description,
			app_name);
	if(rc != SQLITE_OK)
	{
		return rc;
	}

	return res.step();
}

int DB::delete_app(std::string const& app_name) noexcept
{
	sqlite3::statement res;
	int rc = db_.prepare_bind("DELETE FROM app WHERE name = ?", res, app_name);
	if(rc != SQLITE_OK)
	{
		return rc;
	}

	return res.step();
}

}//Agro
