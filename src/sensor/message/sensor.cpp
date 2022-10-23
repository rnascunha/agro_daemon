#include "sensor.hpp"
#include "rapidjson/document.h"
#include "../../message/make.hpp"

namespace Agro{
namespace Sensor{
namespace Message{

static void add_command(rapidjson::Document& doc, sensor_commands command) noexcept
{
	auto const* config = get_config(command);
	if(!config) return;
	doc.AddMember("command",
			rapidjson::Value(config->name, doc.GetAllocator()).Move(),
			doc.GetAllocator());
}

std::string sensor_types_list(Sensor_Type_List const& list) noexcept
{
	rapidjson::Document doc;
	doc.SetObject();

	add_type(doc, ::Message::type::sensor);
	add_command(doc, sensor_commands::list);

	rapidjson::Value data;
	data.SetArray();

	for(auto const& [k, d] : list)
	{
		rapidjson::Value desc;
		desc.SetObject();

		desc.AddMember("id", d.id, doc.GetAllocator());
		desc.AddMember("name",
				rapidjson::Value(d.name.data(), static_cast<rapidjson::SizeType>(d.name.size()), doc.GetAllocator()).Move(),
				doc.GetAllocator());
		desc.AddMember("long_name",
				rapidjson::Value(d.long_name.data(), static_cast<rapidjson::SizeType>(d.long_name.size()), doc.GetAllocator()).Move(),
				doc.GetAllocator());
		desc.AddMember("type", static_cast<int>(d.type), doc.GetAllocator());
		desc.AddMember("unit",
				rapidjson::Value(d.unit.data(), static_cast<rapidjson::SizeType>(d.unit.size()), doc.GetAllocator()).Move(),
				doc.GetAllocator());
		desc.AddMember("unit_name",
				rapidjson::Value(d.unit_name.data(), static_cast<rapidjson::SizeType>(d.unit_name.size()), doc.GetAllocator()).Move(),
				doc.GetAllocator());
		desc.AddMember("description",
				rapidjson::Value(d.description.data(), static_cast<rapidjson::SizeType>(d.description.size()), doc.GetAllocator()).Move(),
				doc.GetAllocator());
		desc.AddMember("add_change",
						d.add_change,
						doc.GetAllocator());

		data.PushBack(desc, doc.GetAllocator());
	}

	::Message::add_data(doc, data);

	return ::Message::stringify(doc);
}

}//Message
}//Sensor
}//Agro
