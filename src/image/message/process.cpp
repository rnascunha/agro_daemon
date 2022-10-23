#include "rapidjson/document.h"

#include "tt/tt.hpp"

#include "../../instance/agro.hpp"
#include "../../websocket/types.hpp"
#include "image.hpp"

//https://github.com/Tencent/rapidjson/issues/1448
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#undef GetObject
#endif

namespace Agro{
namespace Message{

static void delete_images(rapidjson::Document const& doc,
		instance& instance) noexcept
{
	if(!doc.HasMember("data") || !doc["data"].IsArray())
	{
		tt::warning("Image missing 'data' field or wrong type");
		return;
	}

	std::vector<std::string> v;
	std::stringstream ss;
	ss << "Images deleted: ";
	for(auto const& i : doc["data"].GetArray())
	{
		std::string image(i.GetString());
		v.emplace_back(image);
		ss << i.GetString() << ", ";
	}

	instance.delete_image(v);
	instance.send_all_image_list();

	ss.seekp(-2, std::ios_base::end);
	ss << ".";
	instance.notify_all_policy(Authorization::rule::view_image,
			Notify::general_type::image_delete,
			ss.str());
}

static void edit_image(rapidjson::Document const& doc,
		instance& instance) noexcept
{
	if(!doc.HasMember("data") || !doc["data"].IsObject())
	{
		tt::warning("Edit image missing 'data' field or wrong type");
		return;
	}

	rapidjson::Value const& data = doc["data"].GetObject();
	if(!data.HasMember("name") || !data["name"].IsString())
	{
		tt::warning("Edit image missing 'name' field or wrong type");
		return;
	}

	if(!data.HasMember("description") || !data["description"].IsString())
	{
		tt::warning("Edit image missing 'description' field or wrong type");
		return;
	}

	if(!instance.update_image(data["name"].GetString(), data["description"].GetString()))
	{
		tt::error("Error at database editing image.");
		return;
	}

	instance.send_all_image_list();
}

static void download_image(rapidjson::Document const& doc,
		websocket_ptr ws,
		instance& instance) noexcept
{
	if(!doc.HasMember("data") || !doc["data"].IsObject())
	{
		tt::warning("Image request 'data' field missing or wrong type.");
		return;
	}

	rapidjson::Value const& data = doc["data"].GetObject();

	if(!data.HasMember("name") || !data["name"].IsString())
	{
		tt::warning("Image download 'name' field missing or wrong type.");
		return;
	}

	ws->write_file(binary_type::image,
			data["name"].GetString(),
			instance.image_path().make_path(data["name"].GetString()));
}

void process_image(rapidjson::Document const& doc,
		websocket_ptr ws,
		instance& instance) noexcept
{
	if(!doc.HasMember("command") || !doc["command"].IsString())
	{
		tt::warning("Image missing 'command' field or wrong type");
		return;
	}

	auto const* config = get_image_config(doc["command"].GetString());
	if(!config)
	{
		tt::warning("Image command '%s' not found", doc["command"].GetString());
		return;
	}

	switch(config->type)
	{
		case image_commands::erase:
			delete_images(doc, instance);
			break;
		case image_commands::list:
			instance.send_all_image_list();
			break;
		case image_commands::edit:
			edit_image(doc, instance);
			break;
		case image_commands::download:
			download_image(doc, ws, instance);
			break;
		default:
			break;
	}
}

}//Message
}//Agro
