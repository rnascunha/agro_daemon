#include "process.hpp"

#include <string>
#include <iostream>

#include "rapidjson/document.h"

#include "types.hpp"
#include "ota.hpp"

#include "../websocket.hpp"

namespace Message{

void process(engine& coap_engine,
		std::string&& data,
		Device_List& dlist) noexcept
{
	rapidjson::Document d;

	if (d.Parse(data.c_str()).HasParseError() || !d.IsObject())
	{
		std::cerr << "Error parsing receiving message...\n";
		//Not valid message;
		return;
	}

	auto const* typec = Message::get_type_config(d["type"].GetString());
	if(!typec)
	{
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
			process_request(d, dlist, coap_engine);
			break;
		case Message::type::device:
		case Message::type::device_list:
			Websocket<false>::write_all(Message::device_list_to_json(dlist));
			break;
		case Message::type::command:
			process_commands(d, dlist);
			break;
		case Message::type::image:
			process_image(d);
			break;
		case Message::type::info:
			break;
		default:
			break;
	}
}

}//Message
