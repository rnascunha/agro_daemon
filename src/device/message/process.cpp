#include "process.hpp"
#include "device.hpp"

namespace Agro{
namespace Device{
namespace Message{

static void edit_device(rapidjson::Document const& doc,
		Agro::websocket_ptr ws,
		Agro::instance& instance,
		Agro::User::Logged& user) noexcept
{
	if(!Authorization::can(user, Authorization::rule::edit_device))
	{
		tt::warning("User %d can't edit device.", user.id());
		return;
	}

	if(!doc.HasMember("data") || !doc["data"].IsObject())
	{
		tt::warning("Device data not found or wrong type");
		return;
	}

	rapidjson::Value const& data = doc["data"];

	if(!data.HasMember("device") || !data["device"].IsString())
	{
		tt::warning("Missing device!");
		return;
	}

	std::error_code ec;
	std::string addr_str = data["device"].GetString();
	::mesh_addr_t addr{addr_str.data(), static_cast<unsigned>(addr_str.size()), ec};
	if(ec)
	{
		tt::warning("Error parsing device address!");
		return;
	}
	auto* dev = instance.device_list()[addr];
	if(!dev)
	{
		tt::warning("Device not address!");
		return;
	}

	if(data.HasMember("name") && data["name"].IsString())
	{
		dev->name(data["name"].GetString());
		instance.update_db_device(*dev);
		ws->write_policy(Agro::Authorization::rule::view_device,
						std::make_shared<std::string>(Agro::Device::Message::device_edited_to_json(*dev)));
	}
}

void process(rapidjson::Document const& doc,
		Agro::websocket_ptr ws,
		Agro::instance& instance,
		Agro::User::Logged& user) noexcept
{
	if(!doc.HasMember("command") || !doc["command"].IsString())
	{
		tt::warning("Device command not found or wrong type");
		return;
	}

	auto const* config = get_device_config(doc["command"].GetString());
	if(!config)
	{
		tt::warning("Command %s not found!", doc["command"].GetString());
		return;
	}

	switch(config->type)
	{
		case device_commands::list:
		case device_commands::data:
			ws->write_policy(Agro::Authorization::rule::view_device,
						std::make_shared<std::string>(Agro::Device::Message::device_list_to_json(instance.device_list())));
			break;
		case device_commands::edit:
			edit_device(doc, ws, instance, user);
			break;
		case device_commands::request:
			Request::process_request(doc, ws, instance, user);
			break;
		default:
			tt::warning("No callback registered to '%s'", config->name);
			break;
	}
}

}//Message
}//Device
}//Agro
