#include "process.hpp"

#include <string>
#include <iostream>

#include "rapidjson/document.h"

#include "types.hpp"
#include "ota.hpp"
#include "user.hpp"
#include "../device/message/device.hpp"
#include "app.hpp"

#include "../websocket/types.hpp"

namespace Message{

void process(std::string&& data,
		Agro::websocket_ptr ws,
		Agro::instance& instance,
		Agro::User::Logged& user) noexcept
{
	rapidjson::Document d;

	if (d.Parse(data.data(), data.size()).HasParseError() || !d.IsObject())
	{
		std::cerr << "Error parsing receiving message...\n";
		//Not valid message;
		return;
	}

	auto const* typec = Message::get_type_config(d["type"].GetString());
	if(!typec)
	{
		instance.notify_all("Invalid type");
		std::cerr << "Type message not found...\n";
		return;
	}

	std::cout << "Type message: " << static_cast<int>(typec->mtype) << " " << typec->name << "\n";
	switch(typec->mtype)
	{
		case Message::type::resource:
		case Message::type::response:
			break;
		case Message::type::request:
			process_request(d, ws, instance, user);
			break;
		case Message::type::device:
			ws->write_policy(Agro::Authorization::rule::view_device,
					std::make_shared<std::string>(Agro::Device::Message::device_list_to_json(instance.device_list())));
			break;
		case Message::type::command:
			process_commands(d, ws, instance.device_list());
			break;
		case Message::type::image:
			process_image(d, ws);
			break;
		case Message::type::app:
			process_app(d, ws);
			break;
		case Message::type::user:
			process_user(d, ws, instance, user);
			break;
		case Message::type::info:
			break;
		default:
			break;
	}
}

}//Message
