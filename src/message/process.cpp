#include "process.hpp"

#include <string>
#include <iostream>

#include "rapidjson/document.h"

#include "types.hpp"
#include "ota.hpp"
#include "user.hpp"
#include "../device/message/process.hpp"
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
			Agro::Device::Message::process(d, ws, instance, user);
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
