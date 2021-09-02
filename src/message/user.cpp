#include "user.hpp"
#include "../notify/notify.hpp"
#include <iostream>
#include "make.hpp"

namespace Message{

void process_subscription(rapidjson::Document const& doc) noexcept
{
	if(!doc.HasMember("data") || !doc["data"].IsObject())
	{
		std::cerr << "User subscription data not found or not correct type. Exiting.\n";
		return;
	}
	rapidjson::Value const& data = doc["data"];
	if(!data.HasMember("is_subscribed") || !data["is_subscribed"].IsBool())
	{
		std::cerr << "Error parsing file\n";
		return;
	}
	if(data["is_subscribed"].GetBool())
	{
		if(!data.HasMember("subscription") || !data["subscription"].IsObject())
		{
			std::cerr << "Error parsing file\n";
			return;
		}
		rapidjson::Value const& sub = data["subscription"];
		if(!sub.HasMember("endpoint") || !sub["endpoint"].IsString())
		{
			std::cerr << "Error parsing file\n";
			return;
		}
		const char* endpoint = sub["endpoint"].GetString();
		if(!sub.HasMember("keys") || !sub["keys"].IsObject())
		{
			std::cerr << "Error parsing file\n";
			return;
		}
		rapidjson::Value const& keys = sub["keys"];
		if(!keys.HasMember("p256dh") || !keys["p256dh"].IsString()
			|| !keys.HasMember("auth") || !keys["auth"].IsString())
		{
			std::cerr << "Error parsing file\n";
			return;
		}
		const char *p256dh = keys["p256dh"].GetString(),
					*auth = keys["auth"].GetString();
		add_subscription(endpoint, p256dh, auth);
	}
	else
	{
		remove_subscription();
	}
}

void process_user(rapidjson::Document const& doc) noexcept
{
	if(!doc.HasMember("command") || !doc["command"].IsString())
	{
		std::cerr << "User field 'command' not found or not correct type. Exiting.\n";
		return;
	}
	config<user_commands> const* config =  get_user_config(doc["command"].GetString());
	if(!config)
	{
		std::cerr << "User 'command' not supported [" << doc["command"].GetString() << "]. Exiting.\n";
		return;
	}

	switch(config->mtype)
	{
		case user_commands::subscription:
			process_subscription(doc);
		break;
		case user_commands::logout:
		break;
		default:
			std::cerr << "User 'command' " << config->name << " has no process function.\n";
		break;
	}
}

std::string user_error_authentication(std::error_code const& ec) noexcept
{
	rapidjson::Document doc;
	doc.SetObject();
	add_type(doc, type::user);
	doc.AddMember("command",
			rapidjson::Value("authenticate", doc.GetAllocator()).Move(),
			doc.GetAllocator());

	rapidjson::Value data;
	data.SetObject();
	data.AddMember("authenticated", false, doc.GetAllocator());
	data.AddMember("error_message",
			rapidjson::Value(ec.message().data(), ec.message().size(), doc.GetAllocator()).Move(),
			doc.GetAllocator());

	add_data(doc, data);

	return stringify(doc);
}

std::string user_authentication(Agro::User const& user) noexcept
{
	rapidjson::Document doc;
	doc.SetObject();
	add_type(doc, type::user);
	doc.AddMember("command",
			rapidjson::Value("authenticate", doc.GetAllocator()).Move(),
			doc.GetAllocator());

	rapidjson::Value data;
	data.SetObject();
	data.AddMember("authenticated", true, doc.GetAllocator());
	data.AddMember("name",
			rapidjson::Value(user.name().data(), user.name().size(), doc.GetAllocator()).Move(),
			doc.GetAllocator());
	data.AddMember("email",
				rapidjson::Value(user.email().data(), user.email().size(), doc.GetAllocator()).Move(),
				doc.GetAllocator());
	data.AddMember("sessionid",
			rapidjson::Value(user.session_id().data(), user.session_id().size(), doc.GetAllocator()).Move(),
			doc.GetAllocator());

	add_data(doc, data);

	return stringify(doc);
}

}//Message
