#include "user.hpp"
#include "user_types.hpp"
//#include "../notify/notify.hpp"
#include "rapidjson/document.h"
#include "make.hpp"
#include "tt/tt.hpp"

namespace Message{

static void add_subscription(
		Agro::User& user,
		Agro::DB& db,
		const char* endpoint,
		const char* p256dh,
		const char* auth) noexcept
{
	tt::status("Subscribing %s(%s) to push notification", user.username().c_str(), user.user_agent().c_str());
	user.push_subscribe(endpoint, p256dh, auth);
	db.push_subscribe_user(user, endpoint, p256dh, auth);
}

static void remove_subcription(Agro::User& user,
		Agro::DB& db) noexcept
{
	tt::status("Unsubscribing %s(%s) to push notification", user.username().c_str(), user.user_agent().c_str());
	user.push_unsubscribe();
	db.push_unsubscribe_user(user);
}

void process_subscription(
		rapidjson::Document const& doc,
		Agro::User& user,
		Agro::DB& db) noexcept
{
	if(!doc.HasMember("data") || !doc["data"].IsObject())
	{
		tt::warning("User subscription data not found or not correct type.");
		return;
	}
	rapidjson::Value const& data = doc["data"];

	if(!data.HasMember("subscription") || !data["subscription"].IsObject())
	{
		tt::warning("Error parsing file ['subscrition' not found].");
		return;
	}
	rapidjson::Value const& sub = data["subscription"];
	if(!sub.HasMember("endpoint") || !sub["endpoint"].IsString())
	{
		tt::warning("Error parsing file ['endpoint' not found]");
		return;
	}
	const char* endpoint = sub["endpoint"].GetString();
	if(!sub.HasMember("keys") || !sub["keys"].IsObject())
	{
		tt::warning("Error parsing file ['keys' not found]");;
		return;
	}
	rapidjson::Value const& keys = sub["keys"];
	if(!keys.HasMember("p256dh") || !keys["p256dh"].IsString()
		|| !keys.HasMember("auth") || !keys["auth"].IsString())
	{
		tt::warning("Error parsing file ['pd256' or 'auth' not found]");
		return;
	}
	const char *p256dh = keys["p256dh"].GetString(),
				*auth = keys["auth"].GetString();

	add_subscription(user, db, endpoint, p256dh, auth);
}

static void process_unsubscription(rapidjson::Document const&,
		Agro::User& user,
		Agro::DB& db) noexcept
{
	remove_subcription(user, db);
}

static void logout(Agro::User const& user,
		Agro::DB& db,
		std::shared_ptr<Websocket<false>> ws) noexcept
{
	tt::debug("User %s (%s) logging out.",
			user.name().c_str(), user.username().c_str());
	db.clear_session_user_agent(user);
	ws->close(1000);	//Normal close
}

void process_user(rapidjson::Document const& doc,
		std::shared_ptr<Websocket<false>> ws,
		Agro::instance& instance,
		Agro::User& user) noexcept
{
	if(!doc.HasMember("command") || !doc["command"].IsString())
	{
		tt::warning("User field 'command' not found or not correct type.");
		return;
	}

	config<user_commands> const* config =  get_user_config(doc["command"].GetString());
	if(!config)
	{
		tt::warning("User 'command' not supported [%s].", doc["command"].GetString());
		return;
	}

	switch(config->mtype)
	{
		case user_commands::push_subscribe:
			process_subscription(doc, user, instance.db);
		break;
		case user_commands::push_unsubscribe:
			process_unsubscription(doc, user, instance.db);
		break;
		case user_commands::logout:
			logout(user, instance.db, ws);
		break;
		case user_commands::autheticate:
		case user_commands::auth_session_id:
			tt::warning("User already authenticated. [%s]", config->name);
			break;
		default:
			tt::warning("User command '%s' has no process function.", config->name);
		break;
	}
}

}//Message
