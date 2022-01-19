#include "process.hpp"

#include "types.hpp"
#include "user.hpp"
#include "../device/message/process.hpp"
#include "../sensor/message/process.hpp"
#include "../notify/message/process.hpp"

namespace Agro{
namespace Message{

void process(std::string&& data,
		websocket_ptr ws,
		instance& instance,
		User::Logged& user) noexcept
{
	rapidjson::Document d;

	if(d.Parse(data.data(), data.size()).HasParseError() || !d.IsObject())
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

	switch(typec->type)
	{
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
		case ::Message::type::sensor:
			Sensor::Message::process(d, ws, instance, user);
			break;
		case ::Message::type::notify:
			Notify::Message::process(d, ws, instance, user);
			break;
		default:
			break;
	}
}

//Process& Process::install(::Message::type type, callback cp) noexcept
//{
//	list_cb_.emplace(type, cp);
//	return *this;
//}
//
//bool Process::exec(::Message::type type,
//					rapidjson::Document const& doc,
//					websocket_ptr ws,
//					instance& instance,
//					User::Logged& user) noexcept
//{
//	auto cb = list_cb_.find(type);
//	if(cb == list_cb_.end())
//	{
//		tt::warning("Type message not found.");
//		return false;
//	}
//	cb->second(doc, ws, instance, user);
//
//	return true;
//}

}//Message
}//Agro
