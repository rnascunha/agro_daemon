#include "ota.hpp"
#include "make.hpp"
#include "../ota/ota.hpp"
#include "../websocket/websocket.hpp"
#include "../helper/sha256.hpp"

namespace Message{

void ota_image_list(rapidjson::Document& doc, std::filesystem::path const& path) noexcept
{
	doc.SetObject();

	add_type(doc, type::image);
	doc.AddMember("command", "list", doc.GetAllocator());
	rapidjson::Value data;
	add_data(doc, make_image_list(path, data, doc.GetAllocator()));
}

std::string ota_image_list(std::filesystem::path const& path) noexcept
{
	rapidjson::Document doc;
	ota_image_list(doc, path);
	return stringify(doc);
}

static void delete_images(rapidjson::Document const& doc) noexcept
{
	if(!doc.HasMember("data") || !doc["data"].IsArray()) return;

	std::vector<std::string> v;
	for(auto const& i : doc["data"].GetArray())
	{
		std::string image(i.GetString());
		v.emplace_back(image);
	}
	delete_image(v);
	Websocket<false>::write_all(ota_image_list(ota_path()));
}

void process_image(rapidjson::Document const& doc) noexcept
{
	if(!doc.HasMember("command") || !doc["command"].IsString()) return;

	config<image_commands> const* config = get_config(doc["command"].GetString());
	if(!config) return;

	switch(config->mtype)
	{
		case image_commands::erase:
			delete_images(doc);
			break;
		case image_commands::list:
			Websocket<false>::write_all(ota_image_list(ota_path()));
			break;
		default:
			break;
	}
}

}//Message
