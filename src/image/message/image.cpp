#include "image.hpp"
#include "../../message/make.hpp"
#include "../image.hpp"
#include "../../helper/sha256.hpp"

#include "impl/image_impl.hpp"

namespace Agro{
namespace Message{

void image_list(rapidjson::Document& doc, std::filesystem::path const& path, instance& instance) noexcept
{
	doc.SetObject();

	::Message::add_type(doc, ::Message::type::image);
	doc.AddMember("command", "list", doc.GetAllocator());
	rapidjson::Value data;

	::Message::add_data(doc, make_image_list(path, instance, data, doc.GetAllocator()));
}

std::string image_list(std::filesystem::path const& path, instance& instance) noexcept
{
	rapidjson::Document doc;
	image_list(doc, path, instance);
	return ::Message::stringify(doc);
}

}//Message
}//Agro
