#include "agro.hpp"
#include "../image/message/image.hpp"

namespace Agro{

Image_Path const& instance::image_path() const noexcept
{
	return image_;
}

bool instance::add_image(std::string const& image_name,
				User::user_id uploader,
				std::string const& description /* = "" */) noexcept
{
	if(db_.add_image(image_name, uploader, description) != SQLITE_DONE)
	{
		tt::error("Error inserting image to database [%s]", image_name.c_str());
		return false;
	}
	return true;
}

void instance::delete_image(std::vector<std::string> const& list) noexcept
{
	image_.erase(list);
	for(auto const& n : list)
	{
		db_.delete_image(n);
	}
}

bool instance::read_image(std::string const& image_name, Image& image) noexcept
{
	return db_.read_image(image_name, image) == SQLITE_ROW;
}

bool instance::update_image(std::string const& image_name,
						std::string const& description) noexcept
{
	return db_.update_image(image_name, description) == SQLITE_DONE;
}

void instance::send_all_image_list() noexcept
{
	share_->write_all_policy(Authorization::rule::view_image,
		std::make_shared<std::string>(Message::image_list(image_path().path(), *this)));
}

}//Agro
