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

static bool read_data_packet(rapidjson::Document const& doc,
							sensor_description& sensor) noexcept
{
	if(!doc.HasMember("data") || !doc["data"].IsObject())
	{
		tt::warning("Sensor 'data' field not found or wrong type");
		return false;
	}

	rapidjson::Value const& data = doc["data"].GetObject();

	if(!data.HasMember("id") || !data["id"].IsInt())
	{
		tt::warning("Sensor 'id' field not found or wrong type");
		return false;
	}
	sensor.id = data["id"].GetInt();

	if(!data.HasMember("name") || !data["name"].IsString())
	{
		tt::warning("Sensor 'name' field not found or wrong type");
		return false;
	}
	sensor.name = data["name"].GetString();

	if(!data.HasMember("long_name") || !data["long_name"].IsString())
	{
		tt::warning("Sensor 'long_name' field not found or wrong type");
		return false;
	}
	sensor.long_name = data["long_name"].GetString();

	if(!data.HasMember("unit") || !data["unit"].IsString())
	{
		tt::warning("Sensor 'unit' field not found or wrong type");
		return false;
	}
	sensor.unit = data["unit"].GetString();

	if(!data.HasMember("unit_name") || !data["unit_name"].IsString())
	{
		tt::warning("Sensor 'unit_name' field not found or wrong type");
		return false;
	}
	sensor.unit_name = data["unit_name"].GetString();

	if(!data.HasMember("description") || !data["description"].IsString())
	{
		tt::warning("Sensor 'description' field not found or wrong type");
		return false;
	}
	sensor.description = data["description"].GetString();

	if(!data.HasMember("unit_type") || !data["unit_type"].IsInt())
	{
		tt::warning("Sensor 'unit_type' field not found or wrong type");
		return false;
	}
	sensor.type = static_cast<sensor_unit_type>(data["unit_type"].GetInt());

	if(!data.HasMember("add_change") || !data["add_change"].IsBool())
	{
		tt::warning("Sensor 'add_change' field not found or wrong type");
		return false;
	}
	sensor.add_change = data["add_change"].GetBool();

	return true;
}

static void add_sensor_type(rapidjson::Document const& doc,
		websocket_ptr ws,
		instance& instance,
		User::Logged&) noexcept
{
	sensor_description sensor;
	if(!read_data_packet(doc, sensor)) return;

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
	sensor_description sensor;
	if(!read_data_packet(doc, sensor)) return;

	if(!instance.update_sensor_type(sensor))
	{
		tt::warning("Erro update sensor type [%s]", sensor.name.c_str());
		return;
	}

	ws->write_all(sensor_types_list(instance.sensor_list()));
}

static void sensor_export(rapidjson::Document const& doc,
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

	if(!data.HasMember("type") || !data["type"].IsUint())
	{
		tt::warning("Sensor 'type' not defined");
		return;
	}

	if(!data.HasMember("index") || !data["index"].IsUint())
	{
		tt::warning("Sensor 'index' not defined");
		return;
	}

	if(!data.HasMember("device_id") || !data["device_id"].IsInt())
	{
		tt::warning("Sensor 'device_id' not defined");
		return;
	}
	unsigned type = data["type"].GetUint(), index = data["index"].GetUint();
	Device::device_id id = data["device_id"].GetInt();

	value_time init = 0, end = get_time();
	if(!data.HasMember("init") || !data["init"].IsUint())
	{
		tt::warning("Sensor export init time not defined. Using default '%u'", init);
	}
	else
	{
		init = data["init"].GetUint();
	}

	if(!data.HasMember("end") || !data["end"].IsUint())
	{
		tt::warning("Sensor export end time not defined. Using default 'now'");
	}
	else
	{
		end = data["end"].GetUint();
	}

	std::string sensor_data = instance.get_sensor_value(id, type, index, init, end);

	std::string name{"sensor.csv"};
	ws->write_binary(binary_type::file, name, sensor_data);
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
		case sensor_commands::exportv:
			sensor_export(doc, ws, instance, user);
			break;
		default:
			tt::warning("No callback registered to '%s'", config->name);
			break;
	}
}

}//Message
}//Sensor
}//Agro
