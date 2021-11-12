#include "process.hpp"
#include "sensor.hpp"

namespace Agro{
namespace Sensor{
namespace Message{

static void remove_sensor_type(rapidjson::Document const& doc,
		websocket_ptr ws,
		instance& instance,
		User::Logged&) noexcept
{
	if(!doc.HasMember("data") || !doc["data"].IsObject())
	{
		tt::warning("Sensor 'data' field not found or wrong type");
		return;
	}

	rapidjson::Value const& data = doc["data"].GetObject();
	if(!data.HasMember("name") || !data["name"].IsString())
	{
		tt::warning("Sensor 'name' field not found or wrong type");
		return;
	}

	std::string name = data["name"].GetString();
	if(!instance.remove_sensor_type(name))
	{
		tt::warning("Erro removing sensor type [%s]", name.c_str());
		return;
	}

	ws->write_all(sensor_types_list(instance.sensor_list()));
}
static void add_sensor_type(rapidjson::Document const& doc,
		websocket_ptr ws,
		instance& instance,
		User::Logged&) noexcept
{
	if(!doc.HasMember("data") || !doc["data"].IsObject())
	{
		tt::warning("Sensor 'data' field not found or wrong type");
		return;
	}

	rapidjson::Value const& data = doc["data"].GetObject();
	sensor_description sensor;

	if(!data.HasMember("id") || !data["id"].IsInt())
	{
		tt::warning("Sensor 'id' field not found or wrong type");
		return;
	}
	sensor.id = data["id"].GetInt();

	if(!data.HasMember("name") || !data["name"].IsString())
	{
		tt::warning("Sensor 'name' field not found or wrong type");
		return;
	}
	sensor.name = data["name"].GetString();

	if(!data.HasMember("long_name") || !data["long_name"].IsString())
	{
		tt::warning("Sensor 'long_name' field not found or wrong type");
		return;
	}
	sensor.long_name = data["long_name"].GetString();

	if(!data.HasMember("unit") || !data["unit"].IsString())
	{
		tt::warning("Sensor 'unit' field not found or wrong type");
		return;
	}
	sensor.unit = data["unit"].GetString();

	if(!data.HasMember("unit_name") || !data["unit_name"].IsString())
	{
		tt::warning("Sensor 'unit_name' field not found or wrong type");
		return;
	}
	sensor.unit_name = data["unit_name"].GetString();

	if(!data.HasMember("description") || !data["description"].IsString())
	{
		tt::warning("Sensor 'description' field not found or wrong type");
		return;
	}
	sensor.description = data["description"].GetString();

	if(!data.HasMember("unit_type") || !data["unit_type"].IsInt())
	{
		tt::warning("Sensor 'unit_type' field not found or wrong type");
		return;
	}
	sensor.type = static_cast<sensor_unit_type>(data["unit_type"].GetInt());

	if(!instance.add_sensor_type(sensor))
	{
		tt::warning("Erro adding sensor type [%s]", sensor.name.c_str());
		return;
	}

	ws->write_all(sensor_types_list(instance.sensor_list()));
}

static void edit_sensor_type(rapidjson::Document const& doc,
		websocket_ptr ws,
		instance& instance,
		User::Logged&) noexcept
{
	if(!doc.HasMember("data") || !doc["data"].IsObject())
	{
		tt::warning("Sensor 'data' field not found or wrong type");
		return;
	}

	rapidjson::Value const& data = doc["data"].GetObject();
	sensor_description sensor;

	if(!data.HasMember("id") || !data["id"].IsInt())
	{
		tt::warning("Sensor 'id' field not found or wrong type");
		return;
	}
	sensor.id = data["id"].GetInt();

	if(!data.HasMember("name") || !data["name"].IsString())
	{
		tt::warning("Sensor 'name' field not found or wrong type");
		return;
	}
	sensor.name = data["name"].GetString();

	if(!data.HasMember("long_name") || !data["long_name"].IsString())
	{
		tt::warning("Sensor 'long_name' field not found or wrong type");
		return;
	}
	sensor.long_name = data["long_name"].GetString();

	if(!data.HasMember("unit") || !data["unit"].IsString())
	{
		tt::warning("Sensor 'unit' field not found or wrong type");
		return;
	}
	sensor.unit = data["unit"].GetString();

	if(!data.HasMember("unit_name") || !data["unit_name"].IsString())
	{
		tt::warning("Sensor 'unit_name' field not found or wrong type");
		return;
	}
	sensor.unit_name = data["unit_name"].GetString();

	if(!data.HasMember("description") || !data["description"].IsString())
	{
		tt::warning("Sensor 'description' field not found or wrong type");
		return;
	}
	sensor.description = data["description"].GetString();

	if(!data.HasMember("unit_type") || !data["unit_type"].IsInt())
	{
		tt::warning("Sensor 'unit_type' field not found or wrong type");
		return;
	}
	sensor.type = static_cast<sensor_unit_type>(data["unit_type"].GetInt());

	if(!instance.update_sensor_type(sensor))
	{
		tt::warning("Erro update sensor type [%s]", sensor.name.c_str());
		return;
	}

	ws->write_all(sensor_types_list(instance.sensor_list()));
}

void process(rapidjson::Document const& doc,
		websocket_ptr ws,
		instance& instance,
		User::Logged& user) noexcept
{
	if(!doc.HasMember("command") || !doc["command"].IsString())
	{
		tt::warning("Sensor command not found or wrong type");
		return;
	}

	auto const* config = get_sensor_config(doc["command"].GetString());
	if(!config)
	{
		tt::warning("Command %s not found!", doc["command"].GetString());
		return;
	}

	switch(config->mtype)
	{
		case sensor_commands::list:
			ws->write(sensor_types_list(instance.sensor_list()));
			break;
		case sensor_commands::add:
			add_sensor_type(doc, ws, instance, user);
			break;
		case sensor_commands::edit:
			edit_sensor_type(doc, ws, instance, user);
			break;
		case sensor_commands::remove:
			remove_sensor_type(doc, ws, instance, user);
			break;
		default:
			tt::warning("No callback registered to '%s'", config->name);
			break;
	}
}

}//Message
}//Sensor
}//Agro
