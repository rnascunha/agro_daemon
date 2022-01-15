#include "db.hpp"
#include "../helper/time_helper.hpp"

namespace Agro{

using xeds::sqlite3;

int DB::add_image(std::string const& image_name,
					User::user_id uploader_id,
					std::string const& description /* = "" */) noexcept
{
	sqlite3::statement res;
	int rc = db_.prepare_bind("INSERT INTO image(name,uploader_id,description,date_upload) VALUES(?,?,?,?)",
			res,
			image_name,
			uploader_id,
			description,
			time_epoch_seconds());

	if(rc != SQLITE_OK)
	{
		return rc;
	}

	rc = res.step();
	return rc;
}

int DB::read_image(std::string const& image_name,
		Image& image) noexcept
{
	sqlite3::statement res;
	int rc = db_.prepare_bind("SELECT imageid,uploader_id,description,date_upload FROM image WHERE name = ?",
			res, image_name);
	if(rc != SQLITE_OK)
	{
		return rc;
	}

	rc = res.step();
	if(rc != SQLITE_ROW)
	{
		return rc;
	}

	image.id = res.integer(0);
	image.uploader = res.integer(1);
	image.description = res.text(2);
	image.time = res.integer(3);

	return rc;
}

int DB::update_image(std::string const& image_name,
		std::string const& description) noexcept
{
	sqlite3::statement res;
	int rc = db_.prepare_bind(
			"UPDATE image SET description = ? WHERE name = ?",
			res,
			description,
			image_name);
	if(rc != SQLITE_OK)
	{
		return rc;
	}

	return res.step();
}

int DB::delete_image(std::string const& image_name) noexcept
{
	sqlite3::statement res;
	int rc = db_.prepare_bind("DELETE FROM image WHERE name = ?", res, image_name);
	if(rc != SQLITE_OK)
	{
		return rc;
	}

	return res.step();
}

}//Agro
