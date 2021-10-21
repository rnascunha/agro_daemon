#include "process.hpp"

#include "rapidjson/document.h"

#include "types.hpp"
#include "user.hpp"
#include "../device/message/process.hpp"

#include "../websocket/types.hpp"

namespace Agro{
namespace Message{

void process(std::string&& data,
		websocket_ptr ws,
		instance& instance,
		User::Logged& user) noexcept
{
	rapidjson::Document d;

	if (d.Parse(data.data(), data.size()).HasParseError() || !d.IsObject())
	{
		tt::error("Error parsing receiving message.");
		return;
	}

	auto const* typec = ::Message::get_type_config(d["type"].GetString());
	if(!typec)
	{
		tt::warning("Type message not found.");
		return;
	}

	switch(typec->mtype)
	{
		case ::Message::type::resource:
		case ::Message::type::response:
			break;
		case ::Message::type::device:
			Device::Message::process(d, ws, instance, user);
			break;
		case ::Message::type::image:
			process_image(d, ws, instance);
			break;
		case ::Message::type::app:
			process_app(d, ws, instance);
			break;
		case ::Message::type::user:
			::Message::process_user(d, ws, instance, user);
			break;
		case ::Message::type::report:
			process_report(d, ws, instance, user);
			break;
		case ::Message::type::info:
			break;
		default:
			break;
	}
}

}//Message
}//Agro
