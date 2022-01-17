#include "rapidjson/document.h"
#include "../../websocket/types.hpp"
#include "../../instance/agro.hpp"
#include "app.hpp"
#include "../app.hpp"

namespace Agro{
namespace Message{

static void delete_apps(rapidjson::Document const& doc,
		Agro::websocket_ptr ws,
		instance& instance) noexcept
{
	if(!doc.HasMember("data") || !doc["data"].IsArray())
	{
		return;
	}

	std::vector<std::string> v;
	for(auto const& i : doc["data"].GetArray())
	{
		std::string image(i.GetString());
		v.emplace_back(image);
	}
	instance.delete_app(v);
	instance.send_all_app_list();
}

static void edit_app(rapidjson::Document const& doc,
		Agro::websocket_ptr ws,
		instance& instance) noexcept
{
	if(!doc.HasMember("data") || !doc["data"].IsObject())
	{
		tt::warning("Edit app missing 'data' field or wrong type");
		return;
	}

	rapidjson::Value const& data = doc["data"].GetObject();
	if(!data.HasMember("name") || !data["name"].IsString())
	{
		tt::warning("Edit app missing 'name' field or wrong type");
		return;
	}

	if(!data.HasMember("description") || !data["description"].IsString())
	{
		tt::warning("Edit app missing 'description' field or wrong type");
		return;
	}

	if(!instance.update_app(data["name"].GetString(), data["description"].GetString()))
	{
		tt::error("Error at database editing image.");
		return;
	}

	instance.send_all_app_list();
}

static void download_app(rapidjson::Document const& doc,
		websocket_ptr ws,
		instance& instance) noexcept
{
	if(!doc.HasMember("data") || !doc["data"].IsObject())
	{
		tt::warning("App request 'data' field missing or wrong type.");
		return;
	}

	rapidjson::Value const& data = doc["data"].GetObject();

	if(!data.HasMember("name") || !data["name"].IsString())
	{
		tt::warning("App download 'name' field missing or wrong type.");
		return;
	}

	ws->write_file(binary_type::app,
			data["name"].GetString(),
			instance.app_path().make_path(data["name"].GetString()));
}

void process_app(rapidjson::Document const& doc,
		websocket_ptr ws,
		instance& instance) noexcept
{
	if(!doc.HasMember("command") || !doc["command"].IsString())
	{
		tt::warning("App request missing 'command' field.");
		return;
	}

	auto const* config = get_app_config(doc["command"].GetString());
	if(!config)
	{
		tt::warning("App command '%s' not found.", doc["command"].GetString());
		return;
	}

	switch(config->type)
	{
		case app_commands::erase:
			delete_apps(doc, ws, instance);
			break;
		case app_commands::list:
			instance.send_all_app_list();
			break;
		case app_commands::edit:
			edit_app(doc, ws, instance);
			break;
		case app_commands::download:
			download_app(doc, ws, instance);
			break;
		default:
			break;
	}
}

}//Message
}//Agro
