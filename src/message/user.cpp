#include "user.hpp"
#include "user_types.hpp"
#include "rapidjson/document.h"
#include "make.hpp"
#include "tt/tt.hpp"
#include "../user/message/user.hpp"

namespace Message{

static void add_subscription(
		Agro::User::Logged const& user,
		Agro::instance& instance,
		const char* endpoint,
		const char* p256dh,
		const char* auth) noexcept
{
	instance.push_subscribe_user(
			user.id(),
			user.user_agent(),
			endpoint, p256dh, auth);
	tt::status("Subscribing %s(%s) to push notification",
			user.user()->info().username().c_str(),
			user.user_agent().c_str());
}

static void remove_subcription(Agro::User::Logged const& user,
		Agro::instance& instance) noexcept
{
	instance.push_unsubscribe_user(user.id(), user.user_agent());
	tt::status("Unsubscribing %s(%s) to push notification",
			user.user()->info().username().c_str(), user.user_agent().c_str());
}

void process_subscription(
		rapidjson::Document const& doc,
		Agro::User::Logged& user,
		Agro::instance& instance) noexcept
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

	if(data.HasMember("user_agent") && data["user_agent"].IsString())
	{
		user.user_agent(data["user_agent"].GetString());
	}

	const char *p256dh = keys["p256dh"].GetString(),
				*auth = keys["auth"].GetString();

	add_subscription(user, instance, endpoint, p256dh, auth);
}

static void process_unsubscription(rapidjson::Document const&,
		Agro::User::Logged& user,
		Agro::instance& instance) noexcept
{
	remove_subcription(user, instance);
}

static void logout(Agro::User::Logged const& user,
		Agro::instance& instance,
		Agro::websocket_ptr ws) noexcept
{
	tt::debug("User %s (%s) logging out.",
			user.user()->info().name().c_str(), user.user()->info().username().c_str());
	instance.clear_session_user_agent(user.id(), user.user_agent());
	ws->close(1000);	//Normal close
}

static void user_group_policies(Agro::websocket_ptr ws,
		Agro::instance& instance,
		Agro::User::Logged& user) noexcept
{
	if(!Agro::Authorization::can(user, Agro::Authorization::rule::user_admin))
	{
		tt::warning("user_group_policies: User %d not allowed", user.id());
		return;
	}

	tt::debug("User %s request all users data.", user.user()->info().username().c_str());
	ws->write(Agro::User::Message::user_group_permissions(user, instance));
}

static void add_user(rapidjson::Document const& doc,
		Agro::websocket_ptr ws,
		Agro::instance& instance,
		Agro::User::Logged& user) noexcept
{
	if(!Agro::Authorization::can(user, Agro::Authorization::rule::user_admin))
	{
		tt::warning("add_user: User %d not allowed", user.id());
		return;
	}

	if(!doc.HasMember("data") || !doc["data"].IsObject())
	{
		tt::warning("'add_user' missing field.");
		return;
	}
	rapidjson::Value const& data = doc["data"];

	if((!data.HasMember("username") || !data["username"].IsString())
		|| (!data.HasMember("name") || !data["name"].IsString())
		|| (!data.HasMember("password") || !data["password"].IsString())
		|| (!data.HasMember("email") || !data["email"].IsString()))
	{
		tt::warning("'add_user' missing field or invalid type.");
		return;
	}

	std::string username = data["username"].GetString(),
				name = data["name"].GetString(),
				password = data["password"].GetString(),
				email =  data["email"].GetString();

	if(username.empty() || password.empty())
	{
		tt::warning("'add_user' empty 'username' or 'password'.");
		return;
	}

	if(name.empty())
	{
		name = username;
	}

	std::vector<Agro::User::group_id> groups;
	if(!data.HasMember("groups") || !data["groups"].IsArray())
	{
		tt::warning("'add_user' group list not found or wrong type");
	}
	else
	{
		for(auto&& group : data["groups"].GetArray())
		{
			groups.push_back(group.GetInt());
		}
	}

	Agro::User::user_id id;
	if(instance.add_user(username, name, password, email, id))
	{
		instance.add_user_to_group(id, groups);

		auto const* user = instance.get_user(id);
		if(!user)
		{
			tt::error("Error retriving added user value");
			return;
		}
		tt::status("New user '%s'(%s) added successfully!",
				user->info().name().c_str(), user->info().username().c_str());
		ws->write_all_policy(Agro::Authorization::rule::user_admin,
				std::make_shared<std::string>(Agro::User::Message::added_new_user(*user, groups)));
	}
}

static void edit_user(rapidjson::Document const& doc,
		Agro::websocket_ptr ws,
		Agro::instance& instance,
		Agro::User::Logged& user) noexcept
{
	if(!doc.HasMember("data") || !doc["data"].IsObject())
	{
		tt::warning("'edit_user' missing field.");
		return;
	}
	rapidjson::Value const& data = doc["data"];

	if((!data.HasMember("id") || !data["id"].IsInt())
		|| (!data.HasMember("username") || !data["username"].IsString())
		|| (!data.HasMember("name") || !data["name"].IsString())
		|| (!data.HasMember("email") || !data["email"].IsString())
		|| (!data.HasMember("groups") || !data["groups"].IsArray()))
	{
		tt::warning("'edit_user' missing field or invalid type.");
		return;
	}

	Agro::User::user_id uid = data["id"].GetInt();
	if(!Agro::Authorization::can(user, Agro::Authorization::rule::user_admin) && uid != user.id())
	{
		tt::warning("edit_user: User %d not allowed", user.id());
		return;
	}

	std::string username = data["username"].GetString(),
				name = data["name"].GetString(),
				email =  data["email"].GetString();
	std::vector<Agro::User::group_id> groups;

	for(auto&& group : data["groups"].GetArray())
	{
		groups.push_back(group.GetInt());
	}

	if(username.empty())
	{
		tt::warning("'edit_user' empty 'username'.");
		return;
	}

	if(name.empty())
	{
		name = username;
	}

	if(instance.edit_user(uid, username, name, email, groups))
	{
		auto const* user = instance.get_user(uid);
		if(!user)
		{
			tt::error("Error retriving editin user value");
			return;
		}
		tt::status("Edited user '%s'(%s) successfully!",
				user->info().name().c_str(), user->info().username().c_str());
		ws->write_all_policy(Agro::Authorization::rule::user_admin,
				std::make_shared<std::string>(Agro::User::Message::edited_user(*user, groups)));
	}
}

static void delete_user(rapidjson::Document const& doc,
		Agro::websocket_ptr ws,
		Agro::instance& instance,
		Agro::User::Logged& user) noexcept
{
	if(!doc.HasMember("data") || !doc["data"].IsObject())
	{
		tt::warning("'delete_user' missing field.");
		return;
	}
	rapidjson::Value const& data = doc["data"];

	if(!data.HasMember("id") || !data["id"].IsInt())
	{
		tt::warning("'delete_user' missing field or wrong type");
		return;
	}

	Agro::User::user_id id = data["id"].GetInt();
	if(!Agro::Authorization::can(user, Agro::Authorization::rule::user_admin) && id != user.id())
	{
		tt::warning("delete_user: User %d not allowed", user.id());
		return;
	}

	tt::debug("Deleting user '%d'", id);
	if(!instance.delete_user(id))
	{
		tt::error("Error deleting user '%d'", id);
		return;
	}

	tt::status("User '%d' delete successfully", id);
	ws->write_all_policy(Agro::Authorization::rule::user_admin,
			std::make_shared<std::string>(Agro::User::Message::deleted_user(id)));
}

static void add_group(rapidjson::Document const& doc,
		Agro::websocket_ptr ws,
		Agro::instance& instance,
		Agro::User::Logged& user) noexcept
{
	if(!Agro::Authorization::can(user, Agro::Authorization::rule::user_admin))
	{
		tt::warning("add_group: User %d not allowed", user.id());
		return;
	}

	if(!doc.HasMember("data") || !doc["data"].IsObject())
	{
		tt::warning("'add_group' missing field.");
		return;
	}
	rapidjson::Value const& data = doc["data"];

	if((!data.HasMember("name") || !data["name"].IsString())
		|| (!data.HasMember("description") || !data["description"].IsString())
		|| (!data.HasMember("members") || !data["members"].IsArray()))
	{
		tt::warning("'add_group' missing field or invalid type.");
		return;
	}

	std::string name = data["name"].GetString(),
				description = data["description"].GetString();

	if(name.empty())
	{
		tt::warning("'add_group' empty 'name'.");
		return;
	}

	std::vector<Agro::User::user_id> members;
	for (auto& m : data["members"].GetArray())
	{
		members.push_back(m.GetInt());
	}

	Agro::User::group_id id;
	if(instance.add_group(name, description, members, id))
	{
		auto const* group = instance.get_group_info(id);
		if(!group)
		{
			tt::error("Error retriving group info");
			return;
		}
		tt::status("New group '%s' added successfully!",
						name.c_str());
		ws->write_all_policy(Agro::Authorization::rule::user_admin,
				std::make_shared<std::string>(Agro::User::Message::added_new_group(*group)));
	}
}

void delete_group(rapidjson::Document const& doc,
		Agro::websocket_ptr ws,
		Agro::instance& instance,
		Agro::User::Logged& user) noexcept
{
	if(!Agro::Authorization::can(user, Agro::Authorization::rule::user_admin))
	{
		tt::warning("delete_group: User %d not allowed", user.id());
		return;
	}

	if(!doc.HasMember("data") || !doc["data"].IsObject())
	{
		tt::warning("'delete_group' missing field.");
		return;
	}
	rapidjson::Value const& data = doc["data"];

	if(!data.HasMember("id") || !data["id"].IsInt())
	{
		tt::warning("'delete_group' missing field or wrong type");
		return;
	}

	Agro::User::user_id id = data["id"].GetInt();
	tt::debug("Deleting group '%d'", id);
	if(!instance.delete_group(id))
	{
		return;
	}

	tt::status("Group '%d' delete successfully", id);
	ws->write_all_policy(Agro::Authorization::rule::user_admin,
			std::make_shared<std::string>(Agro::User::Message::deleted_group(id)));
}

void process_user(rapidjson::Document const& doc,
		Agro::websocket_ptr ws,
		Agro::instance& instance,
		Agro::User::Logged& user) noexcept
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
			process_subscription(doc, user, instance);
		break;
		case user_commands::push_unsubscribe:
			process_unsubscription(doc, user, instance);
		break;
		case user_commands::logout:
			logout(user, instance, ws);
		break;
		case user_commands::autheticate:
		case user_commands::auth_session_id:
			tt::warning("User already authenticated. [%s]", config->name);
			break;
		case user_commands::user_group_policies:
			user_group_policies(ws, instance, user);
			break;
		case user_commands::add_user:
			add_user(doc, ws, instance, user);
			break;
		case user_commands::add_group:
			add_group(doc, ws, instance, user);
			break;
		case user_commands::edit_user:
			edit_user(doc, ws, instance, user);
			break;
		case user_commands::delete_user:
			delete_user(doc, ws, instance, user);
			break;
		case user_commands::delete_group:
			delete_group(doc, ws, instance, user);
			break;
		default:
			tt::warning("User command '%s' has no process function.", config->name);
		break;
	}
}

}//Message
