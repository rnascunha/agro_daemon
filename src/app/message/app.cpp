#include "app.hpp"
#include "../app.hpp"
#include "../../message/make.hpp"

namespace Agro{
namespace Message{

template<typename Allocator>
rapidjson::Value& make_app(rapidjson::Value& data,
		std::string const& name,
		std::size_t size,
		App const& app,
		Allocator alloc) noexcept
{
	data.SetObject();

	data.AddMember("id", app.id, alloc);
	data.AddMember("name", rapidjson::Value(name.c_str(), alloc).Move(), alloc);
	data.AddMember("size", size, alloc);
	data.AddMember("hash", rapidjson::Value(sha256_string(app.hash).c_str(), alloc).Move(), alloc);
	data.AddMember("uploader", app.uploader, alloc);
	data.AddMember("date_upload", app.time, alloc);
	data.AddMember("description",
			rapidjson::Value(app.description.data(), app.description.size(), alloc).Move(),
			alloc);

	return data;
}

template<typename Allocator>
rapidjson::Value& make_app_list(std::filesystem::path const& path,
		instance& instance,
		rapidjson::Value& data, Allocator alloc) noexcept
{
	data.SetArray();
	for(auto const& f : std::filesystem::directory_iterator(path))
	{
		std::size_t size = std::filesystem::file_size(f);

		std::string name = f.path();
		name = name.substr(name.find_last_of('/') + 1);

//		sha256_hash hash;
//		calculate_app_hash(f.path(), hash);
		App app;
		if(!instance.read_app(name, app))
		{
			tt::error("Error reading app '%s' info from database", name.c_str());
			continue;
		}

		rapidjson::Value data_app;
		data.PushBack(make_app(data_app, name, size, app, alloc), alloc);
	}

	return data;
}

void app_list(rapidjson::Document& doc, std::filesystem::path const& path, instance& instance) noexcept
{
	doc.SetObject();

	::Message::add_type(doc, ::Message::type::app);
	doc.AddMember("command", "list", doc.GetAllocator());
	rapidjson::Value data;
	::Message::add_data(doc, make_app_list(path, instance, data, doc.GetAllocator()));
}

std::string app_list(std::filesystem::path const& path, instance& instance) noexcept
{
	rapidjson::Document doc;
	app_list(doc, path, instance);
	return ::Message::stringify(doc);
}

}//Message
}//Agro
