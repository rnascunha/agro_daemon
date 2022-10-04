#include "make.hpp"
#include "notify.hpp"
#include "../../message/make.hpp"

namespace Agro{
namespace Notify{
namespace Message{

template<typename Allocator>
static bool add_command(rapidjson::Value& data, notify_commands command, Allocator alloc) noexcept
{
	auto const* config = get_config(command);
	if(!config) return false;

	data.AddMember("command", rapidjson::Value(config->name, alloc).Move(), alloc);

	return true;
}

template<typename Allocator>
static bool make_notify(rapidjson::Value& data, general_type type, Allocator alloc) noexcept
{
	auto const* config = get_config(type);
	if(!config) return false;

	data.SetObject();
	data.AddMember("name", rapidjson::Value(config->name, alloc).Move(), alloc);

	return true;
}

template<typename Allocator>
static void make_notify_array(rapidjson::Value& data, general_type type, Allocator alloc) noexcept
{
	data.SetArray();

	int type_i = static_cast<int>(type);
	for(std::size_t i = 0; i < (sizeof(type) * 8); i++)
	{
		if(type_i & 1)
		{
			rapidjson::Value noti;
			if(make_notify(noti, static_cast<general_type>(1 << i), alloc))
			{
				data.PushBack(noti, alloc);
			}
		}
		type_i >>= 1;
		if(!type_i) break;
	}
}

std::string make_list(Notify const& notify) noexcept
{
	rapidjson::Document doc;
	doc.SetObject();

	::Message::add_type(doc, ::Message::type::notify);
	add_command(doc, notify_commands::general_list, doc.GetAllocator());

	rapidjson::Value data;
	make_notify_array(data, notify.get(), doc.GetAllocator());

	::Message::add_data(doc, data);

	return ::Message::stringify(doc);
}

template<typename Allocator>
static void make_notify_device(rapidjson::Value& data,
		device_type type,
		Allocator alloc) noexcept
{
	data.SetArray();

	int type_i = static_cast<int>(type);
	for(std::size_t i = 0; i < (sizeof(type) * 8); i++)
	{
		if(type_i & 1)
		{
			auto const* config = get_config(static_cast<device_type>(1 << i));
			if(config)
			{
				data.PushBack(rapidjson::Value(config->name, alloc).Move(), alloc);
			}
		}

		type_i >>= 1;
		if(!type_i) break;
	}
}

template<typename Allocator>
static void make_notify_devices_array(rapidjson::Value& data,
		Notify::device_container const& container,
		Allocator alloc) noexcept
{
	data.SetArray();
	for(auto const& [device, type] : container)
	{
		rapidjson::Value notify_data;
		make_notify_device(notify_data, type, alloc);
		if(notify_data.Size())
		{
			rapidjson::Value dev_data;
			dev_data.SetObject();

			dev_data.AddMember("id", device, alloc);
			dev_data.AddMember("notify", notify_data, alloc);

			data.PushBack(dev_data, alloc);
		}
	}
}

std::string make_device_list(Notify const& notify) noexcept
{
	rapidjson::Document doc;
	doc.SetObject();

	::Message::add_type(doc, ::Message::type::notify);
	add_command(doc, notify_commands::device_list, doc.GetAllocator());

	rapidjson::Value data;
	make_notify_devices_array(data, notify.get_devices(), doc.GetAllocator());

	::Message::add_data(doc, data);

	return ::Message::stringify(doc);
}

template<typename Allocator>
void make_sensor(rapidjson::Value& sen,
		Notify::sensor_container_key const& key,
		Notify::sensor_container_value const& value,
		Allocator alloc,
		bool add_id = true)
{
	sen.SetObject();

	if(add_id)
	{
		sen.AddMember("id", std::get<0>(key), alloc);
	}
	sen.AddMember("type", std::get<1>(key), alloc);
	sen.AddMember("index", std::get<2>(key), alloc);

	rapidjson::Value notis;
	notis.SetArray();
	for(auto const& v : value)
	{
		auto const* config = get_config(v.stype);
		if(!config) continue;

		rapidjson::Value noti;
		noti.SetObject();

		noti.AddMember("enabled", v.enabled, alloc);
		noti.AddMember("operation",
				rapidjson::Value(config->name, alloc).Move(),
				alloc);
		noti.AddMember("value", v.value, alloc);

		notis.PushBack(noti, alloc);
	}
	sen.AddMember("notify", notis, alloc);
}

std::string make_sensor_list(Notify const& notify) noexcept
{
	rapidjson::Document doc;
	doc.SetObject();

	::Message::add_type(doc, ::Message::type::notify);
	add_command(doc, notify_commands::sensor_list, doc.GetAllocator());

	rapidjson::Value data;
	data.SetArray();
	for(auto const& [key, value] : notify.get_sensors())
	{
		rapidjson::Value sen;
		make_sensor(sen, key, value, doc.GetAllocator());
		data.PushBack(sen, doc.GetAllocator());
	}

	::Message::add_data(doc, data);

	return ::Message::stringify(doc);
}

std::string make_sensor_device_list(Device::device_id id, Notify const& notify) noexcept
{
	rapidjson::Document doc;
	doc.SetObject();

	::Message::add_type(doc, ::Message::type::notify);
	add_command(doc, notify_commands::sensor_list, doc.GetAllocator());

	rapidjson::Value data;
	data.SetObject();

	data.AddMember("id", id, doc.GetAllocator());

	rapidjson::Value sensors;
	sensors.SetArray();
	for(auto const& [key, value] : notify.get_sensors())
	{
		if(std::get<0>(key) == id)
		{
			rapidjson::Value sen;
			make_sensor(sen, key, value, doc.GetAllocator(), false);
			sensors.PushBack(sen, doc.GetAllocator());
		}
	}
	data.AddMember("sensors", sensors, doc.GetAllocator());

	::Message::add_data(doc, data);

	return ::Message::stringify(doc);
}

std::string make_public_key(std::string_view const& public_key) noexcept
{
	rapidjson::Document doc;
	doc.SetObject();

	::Message::add_type(doc, ::Message::type::user);
	doc.AddMember("command", rapidjson::Value("notify_key", doc.GetAllocator()).Move(), doc.GetAllocator());

	rapidjson::Value data;
	data.SetObject();

	data.AddMember("key",
			rapidjson::Value(public_key.data(), static_cast<int>(public_key.size()), doc.GetAllocator()).Move(),
			doc.GetAllocator());
	::Message::add_data(doc, data);

	return ::Message::stringify(doc);
}

#define STRING_TO_JVALUE(m)		rapidjson::Value(m.data(), m.size(), alloc).Move()

template<typename Allocator>
static void make_mail_credential(rapidjson::Value& data, mail const& mfac, Allocator& alloc) noexcept
{
	data.SetObject();

	data.AddMember("server", STRING_TO_JVALUE(mfac.server().server), alloc);
	data.AddMember("port", STRING_TO_JVALUE(mfac.server().port), alloc);
	data.AddMember("user", STRING_TO_JVALUE(mfac.server().user), alloc);
	data.AddMember("password", STRING_TO_JVALUE(mfac.server().password), alloc);
}

template<typename Allocator>
static void make_push_credential(rapidjson::Value& data, push const& pfac, Allocator& alloc) noexcept
{
	data.SetObject();

//	data.AddMember("key", STRING_TO_JVALUE(pfac.public_key()), alloc);
}

template<typename Allocator>
static void make_telegram_credential(rapidjson::Value& data, telegram const& tfac, Allocator& alloc) noexcept
{
	data.SetObject();

	data.AddMember("token", STRING_TO_JVALUE(tfac.token()), alloc);
}

std::string make_credential_list(Factory const& fac) noexcept
{
	rapidjson::Document doc;
	doc.SetObject();

	::Message::add_type(doc, ::Message::type::notify);
	add_command(doc, notify_commands::credential_list, doc.GetAllocator());

	rapidjson::Value data;
	data.SetObject();

	for(auto const& [name, noti] : fac)
	{
		rapidjson::Value v;
		if(name == "mail")
		{
			make_mail_credential(v, *static_cast<mail const*>(noti.get()), doc.GetAllocator());
		}
		else if(name == "telegram")
		{
			make_telegram_credential(v, *static_cast<telegram const*>(noti.get()), doc.GetAllocator());
		}
		else if(name == "push")
		{
			make_push_credential(v, *static_cast<push const*>(noti.get()), doc.GetAllocator());
		}
		else
		{
			continue;
		}
		v.AddMember("enable", noti->enable(), doc.GetAllocator());

		data.AddMember(rapidjson::Value(name.data(), static_cast<int>(name.size()), doc.GetAllocator()).Move(), v, doc.GetAllocator());
	}

	::Message::add_data(doc, data);

	return ::Message::stringify(doc);
}

}//Message
}//Notify
}//Agro
