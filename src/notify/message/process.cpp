#include "process.hpp"
#include "notify.hpp"
#include "make.hpp"
#include "../../helper/enum_ops.hpp"

#include <cstring>

//https://github.com/Tencent/rapidjson/issues/1448
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#undef GetObject
#endif

namespace Agro{
namespace Notify{
namespace Message{

static general_type read_type(rapidjson::Value const& data) noexcept
{
	if(!data.HasMember("name") || !data["name"].IsString())
	{
		return general_type::no_alert;
	}

	auto const* config = get_general_type_config(data["name"].GetString());
	if(!config)
	{
		return general_type::no_alert;
	}

	return config->type;
}

static general_type read_types(rapidjson::Value const& data) noexcept
{
	general_type type = general_type::no_alert;
	for(auto const& d : data.GetArray())
	{
		type |= read_type(d);
	}

	return type;
}

static device_type read_device_type(rapidjson::Value const& data) noexcept
{
	auto const* config = get_device_type_config(data.GetString());
	if(!config)
	{
		return device_type::no_alert;
	}

	return config->type;
}

static device_type read_device_types(rapidjson::Value const& data) noexcept
{
	device_type type = device_type::no_alert;
	for(auto const& d : data.GetArray())
	{
		type |= read_device_type(d);
	}

	return type;
}

static void set_general_notify(rapidjson::Document const& doc,
				Agro::websocket_ptr ws,
				Agro::instance& instance,
				Agro::User::Logged& user) noexcept
{
	if(!doc.HasMember("data") || !doc["data"].IsArray())
	{
		tt::warning("Missing 'data' field or wrong type");
		return;
	}

	general_type type = read_types(doc["data"]);

	tt::debug("Updating user '%d' notify list", user.id());
	if(instance.update_general_notify(*user.user(), type))
	{
		ws->write(make_list(user.user()->notify()));

		std::stringstream ss;
		ss << static_cast< int>(type);
		ws->write(instance.make_report(Agro::Message::report_commands::notify,
						Agro::Message::report_type::success,
						"general notify",
						"Updated",
						ss.str(),
						ws->user().id()));
	}
}

static void send_general_list(Agro::websocket_ptr ws,
				Agro::User::Logged& user) noexcept
{
	tt::debug("Sending user '%d' notify list", user.id());
	ws->write(make_list(user.user()->notify()));
}

static void send_device_list(Agro::websocket_ptr ws,
				Agro::User::Logged& user) noexcept
{
	tt::debug("Sending user '%d' notify device list", user.id());
	ws->write(make_device_list(user.user()->notify()));
}

static void set_device_notify(rapidjson::Document const& doc,
				Agro::websocket_ptr ws,
				Agro::instance& instance,
				Agro::User::Logged& user) noexcept
{
	if(!doc.HasMember("data") || !doc["data"].IsObject())
	{
		tt::warning("Missing 'data' field or wrong type");
		return;
	}

	rapidjson::Value const& data = doc["data"];

	if(!data.HasMember("id") || !data["id"].IsUint())
	{
		tt::warning("Missing 'id' field or wrong type");
		return;
	}

	auto device_id = data["id"].GetUint();
	device_type type = read_device_types(data["notify"]);

	tt::debug("Updating user '%d' notify device list", user.id());
	user.user()->notify().set(device_id, type);
	if(instance.update_device_notify(*user.user(), device_id, type))
	{
		ws->write(make_device_list(user.user()->notify()));

		std::stringstream ss;
		ss << device_id << " / " << static_cast<int>(type);
		ws->write(instance.make_report(Agro::Message::report_commands::notify,
				Agro::Message::report_type::success,
				"device notify",
				"Updated",
				ss.str(),
				ws->user().id()));
	}
}

static void set_sensor_notify(rapidjson::Document const& doc,
				Agro::websocket_ptr ws,
				Agro::instance& instance,
				Agro::User::Logged& user) noexcept
{
	if(!doc.HasMember("data") || !doc["data"].IsObject())
	{
		tt::warning("Missing 'data' field or wrong type");
		return;
	}
	rapidjson::Value const& data = doc["data"];

	if(!data.HasMember("id") || !data["id"].IsUint())
	{
		tt::warning("Missing 'id' field or wrong type");
		return;
	}
	auto device_id = data["id"].GetUint();

	if(!data.HasMember("sensors") || !data["sensors"].IsArray())
	{
		tt::warning("Missing 'sensors' field or wrong type");
		return;
	}
	rapidjson::Value const& sensors = data["sensors"];

	std::vector<sensor_temp_notify> v;
	for(auto const& s : sensors.GetArray())
	{
		if(!s.HasMember("enable") || !s["enable"].IsBool())
		{
			tt::warning("Missing 'enable' field or wrong type");
			continue;
		}
		if(!s.HasMember("sensor") || !s["sensor"].IsObject())
		{
			tt::warning("Missing 'sensor' field or wrong type");
			continue;
		}
		if(!s["sensor"].HasMember("type") || !s["sensor"]["type"].IsUint())
		{
			tt::warning("Missing 'sensor/type' field or wrong type");
			continue;
		}
		if(!s["sensor"].HasMember("index") || !s["sensor"]["index"].IsUint())
		{
			tt::warning("Missing 'sensor/index' field or wrong type");
			continue;
		}
		if(!s.HasMember("operation") || !s["operation"].IsString())
		{
			tt::warning("Missing 'operation' field or wrong type");
			continue;
		}
		auto const* config = get_sensor_type_config(s["operation"].GetString());
		if(!config)
		{
			tt::warning("Operation '%s' not found", s["operation"].GetString());
			continue;
		}

		if(!s.HasMember("value") || !s["value"].IsNumber())
		{
			tt::warning("Missing 'value' field or wrong type");
			continue;
		}

		v.emplace_back(
				s["sensor"]["type"].GetUint(),
				s["sensor"]["index"].GetUint(),
				s["enable"].GetBool(),
				config->type,
				s["value"].GetFloat(),
				false);
	}

	tt::debug("Updating user '%d' notify sensor list [%zu]", user.id(), v.size());

	if(instance.update_sensor_notify(*user.user(), device_id, v))
	{
		ws->write(make_sensor_device_list(device_id, user.user()->notify()));

		std::stringstream ss;
		ss << device_id;
		ws->write(instance.make_report(Agro::Message::report_commands::notify,
				Agro::Message::report_type::success,
				"sensor notify",
				"Updated",
				ss.str(),
				ws->user().id()));
	}
}

static void send_sensor_list(Agro::websocket_ptr ws,
				Agro::User::Logged& user) noexcept
{
	tt::debug("Sending user '%d' notify sensor list", user.id());
	ws->write(make_sensor_list(user.user()->notify()));
}

static void send_credential_list(rapidjson::Document const&,
				Agro::websocket_ptr ws,
				Agro::instance& instance,
				Agro::User::Logged& user) noexcept
{
	if(!Agro::Authorization::can(user, Agro::Authorization::rule::user_admin))
	{
		tt::warning("user_group_policies: User %d not allowed", user.id());
		return;
	}
	tt::debug("Sending user '%d' notify credential list", user.id());
	ws->write(make_credential_list(instance.notify()));
}

static void update_mail_credential(rapidjson::Value const& data,
		Agro::instance& instance) noexcept
{
	tt::debug("Updating mail notify");

	if((!data.HasMember("server") || !data["server"].IsString())
		|| (!data.HasMember("port") || !data["port"].IsString())
		|| (!data.HasMember("user") || !data["user"].IsString())
		|| (!data.HasMember("password") || !data["password"].IsString())
		|| (!data.HasMember("enable") || !data["enable"].IsBool())
	)
	{
		tt::warning("Missing field or wrong type updating mail notify");
		return;
	}

	SMTP::server nserver;
	nserver.addr = data["server"].GetString();
	nserver.port = data["port"].GetString();
	nserver.user = data["user"].GetString();
	nserver.password = data["password"].GetString();

	instance.update_mail_credential(nserver, data["enable"].GetBool());
}

static void update_push_credential(rapidjson::Value const& data,
		Agro::instance& instance) noexcept
{
	tt::debug("Updating push notify");

	if(!data.HasMember("enable") || !data["enable"].IsBool())
	{
		tt::warning("Missing field or wrong type updating push notify");
		return;
	}

	instance.update_push_credential(data["enable"].GetBool());
}

static void update_telegram_credential(rapidjson::Value const& data,
		Agro::instance& instance) noexcept
{
	tt::debug("Updating telegram notify");

	if((!data.HasMember("token") || !data["token"].IsString())
		|| (!data.HasMember("enable") || !data["enable"].IsBool())
	)
	{
		tt::warning("Missing field or wrong type updating telegram notify");
		return;
	}

	instance.update_telegram_credential(data["token"].GetString(),
			data["enable"].GetBool());
}

static void update_credential(rapidjson::Document const& doc,
				Agro::websocket_ptr ws,
				Agro::instance& instance,
				Agro::User::Logged& user) noexcept
{
	if(!Agro::Authorization::can(user, Agro::Authorization::rule::user_admin))
	{
		tt::warning("user_group_policies: User %d not allowed", user.id());
		return;
	}

	if(!doc.HasMember("data") || !doc["data"].IsObject())
	{
		tt::warning("Missing 'data' field or wrong type");
		return;
	}
	rapidjson::Value const& data = doc["data"];
	for(auto const& d : data.GetObject())
	{
		std::string key = d.name.GetString();
		if(!d.value.IsObject()) continue;
		if(key == "mail")
		{
			update_mail_credential(d.value.GetObject(), instance);
		}
		else if(key == "push")
		{
			update_push_credential(d.value.GetObject(), instance);
		}
		else if(key == "telegram")
		{
			update_telegram_credential(d.value.GetObject(), instance);
		}
	}

	send_credential_list(doc, ws, instance, user);
	ws->write(instance.make_report(Agro::Message::report_commands::notify,
						Agro::Message::report_type::success,
					"notify credential",
					"Updated",
					"",
					ws->user().id()));
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

	auto const* config = get_notify_config(doc["command"].GetString());
	if(!config)
	{
		tt::warning("Command %s not found!", doc["command"].GetString());
		return;
	}

	switch(config->type)
	{
		case notify_commands::general_list:
			send_general_list(ws, user);
			break;
		case notify_commands::general_set:
			set_general_notify(doc, ws, instance, user);
			break;
		case notify_commands::device_list:
			send_device_list(ws, user);
			break;
		case notify_commands::device_set:
			set_device_notify(doc, ws, instance, user);
			break;
		case notify_commands::sensor_list:
			send_sensor_list(ws, user);
			break;
		case notify_commands::sensor_set:
			set_sensor_notify(doc, ws, instance, user);
			break;
		case notify_commands::credential_list:
			send_credential_list(doc, ws, instance, user);
			break;
		case notify_commands::update_credential:
			update_credential(doc, ws, instance, user);
			break;
		default:
			tt::warning("No callback registered to '%s'", config->name);
			break;
	}
}

}//Message
}//Notify
}//Agro
