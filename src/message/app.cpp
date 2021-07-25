#include "app.hpp"
#include "types.hpp"
#include "../app/app.hpp"
#include "../websocket.hpp"

namespace Message{

template<typename Allocator>
rapidjson::Value& make_app(rapidjson::Value& data,
		std::string const& name,
		std::size_t size,
		sha256_hash const& hash,
		Allocator alloc) noexcept
{
	data.SetObject();

	data.AddMember("name", rapidjson::Value(name.c_str(), alloc).Move(), alloc);
	data.AddMember("size", size, alloc);
	data.AddMember("hash", rapidjson::Value(sha256_string(hash).c_str(), alloc).Move(), alloc);

	return data;
}

template<typename Allocator>
rapidjson::Value& make_app_list(std::filesystem::path const& path,
		rapidjson::Value& data, Allocator alloc) noexcept
{
	data.SetArray();
	for(auto const& f : std::filesystem::directory_iterator(path))
	{
		std::size_t size = std::filesystem::file_size(f);

		std::string name = f.path();
		name = name.substr(name.find_last_of('/') + 1);

		sha256_hash hash;
		calculate_app_hash(f.path(), hash);

		rapidjson::Value app;
		data.PushBack(make_app(app, name, size, hash, alloc), alloc);
	}

	return data;
}

void app_list(rapidjson::Document& doc, std::filesystem::path const& path) noexcept
{
	doc.SetObject();

	add_type(doc, type::app);
	doc.AddMember("command", "list", doc.GetAllocator());
	rapidjson::Value data;
	add_data(doc, make_app_list(path, data, doc.GetAllocator()));
}

std::string app_list(std::filesystem::path const& path) noexcept
{
	rapidjson::Document doc;
	app_list(doc, path);
	return stringify(doc);
}

static void delete_apps(rapidjson::Document const& doc) noexcept
{
	if(!doc.HasMember("data") || !doc["data"].IsArray()) return;

	std::vector<std::string> v;
	for(auto const& i : doc["data"].GetArray())
	{
		std::string image(i.GetString());
		v.emplace_back(image);
	}
	delete_app(v);
	Websocket<false>::write_all(app_list(app_path()));
}

void process_app(rapidjson::Document const& doc) noexcept
{
	if(!doc.HasMember("command") || !doc["command"].IsString()) return;

	config<app_commands> const* config = get_app_config(doc["command"].GetString());
	if(!config) return;

	switch(config->mtype)
	{
		case app_commands::erase:
			delete_apps(doc);
			break;
		case app_commands::list:
			Websocket<false>::write_all(app_list(app_path()));
			break;
		default:
			break;
	}
}


}//Message
