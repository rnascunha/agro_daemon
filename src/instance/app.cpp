#include "agro.hpp"
#include "tt/tt.hpp"
#include "../app/message/app.hpp"

namespace Agro{

App_Path const& instance::app_path() const noexcept
{
	return app_;
}
bool instance::add_app(std::string const& app_name,
		User::user_id uploader,
		std::string const& description /* = "" */) noexcept
{
	sha256_hash hash;
	calculate_app_hash(app_, app_name, hash);

	if(db_.add_app(app_name, uploader, hash, description) != SQLITE_DONE)
	{
		tt::error("Error inserting app to database [%s]", app_name.c_str());
		return false;
	}
	return true;
}

void instance::delete_app(std::vector<std::string> const& list) noexcept
{
	app_.erase(list);
	for(auto const& n : list)
	{
		db_.delete_app(n);
	}
}

bool instance::read_app(std::string const& app_name, App& app) noexcept
{
	return db_.read_app(app_name, app) == SQLITE_ROW;
}

bool instance::update_app(std::string const& app_name,
				std::string const& description) noexcept
{
	return db_.update_app(app_name, description) == SQLITE_DONE;
}

void instance::send_all_app_list() noexcept
{
	share_->write_all_policy(Authorization::rule::view_image,
		std::make_shared<std::string>(Message::app_list(app_path().path(), *this)));
}

}//Agro
