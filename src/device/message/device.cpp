#include "device.hpp"
#include "../../message/make.hpp"
#include "../net.hpp"
#include "esp_reset_reason.hpp"

namespace Agro{
namespace Device{
namespace Message{

template<typename Allocator>
static void make_config_data(rapidjson::Value& data, Device const& dev, Allocator& alloc) noexcept;
template<typename Allocator>
static void make_route_data(rapidjson::Value& data, Device const& dev, Allocator& alloc) noexcept;
template<typename Allocator>
static void make_full_config_data(rapidjson::Value& data, Device const& dev, Allocator& alloc) noexcept;
template<typename Allocator>
static void make_board_data(rapidjson::Value& data, Device const& dev, Allocator& alloc) noexcept;
template<typename Allocator>
static void make_others_data(rapidjson::Value& data, Device const& dev, Allocator& alloc) noexcept;
template<typename Allocator>
static void make_job(rapidjson::Value& data, job const&, Allocator& alloc) noexcept;
template<typename Allocator>
static void make_jobs(rapidjson::Value& data, Device const& dev, Allocator& alloc) noexcept;
template<typename Allocator>
static void make_apps(rapidjson::Value& data, Device const& dev, Allocator& alloc) noexcept;
template<typename Allocator>
static void make_device_data(rapidjson::Value& data, Device const& dev, Allocator& alloc) noexcept;
template<typename Allocator>
static void make_mandatory_data_info(rapidjson::Value&, Device const&, Allocator&) noexcept;
static bool add_command(rapidjson::Document& doc, device_commands comm) noexcept;
template<typename Allocator>
void add_node(rapidjson::Value& data, Tree::node const& node, Allocator& alloc) noexcept;

void device_config_to_json(rapidjson::Document& doc, Device const& dev) noexcept
{
	doc.SetObject();
	add_type(doc, ::Message::type::device);
	add_command(doc, device_commands::data);

	rapidjson::Value data;
	data.SetObject();

	make_mandatory_data_info(data, dev, doc.GetAllocator());

	make_config_data(data, dev, doc.GetAllocator());
	::Message::add_data(doc, data);
}

std::string device_config_to_json(Device const& dev) noexcept
{
	rapidjson::Document doc;
	device_config_to_json(doc, dev);

	return ::Message::stringify(doc);
}

void device_route_to_json(rapidjson::Document& doc, Device const& dev) noexcept
{
	doc.SetObject();
	add_type(doc, ::Message::type::device);
	add_command(doc, device_commands::data);

	rapidjson::Value data;
	data.SetObject();

	make_mandatory_data_info(data, dev, doc.GetAllocator());

	make_route_data(data, dev, doc.GetAllocator());
	::Message::add_data(doc, data);
}

std::string device_route_to_json(Device const& dev) noexcept
{
	rapidjson::Document doc;
	device_route_to_json(doc, dev);

	return ::Message::stringify(doc);
}

void device_full_config_to_json(rapidjson::Document& doc, Device const& dev) noexcept
{
	doc.SetObject();
	add_type(doc, ::Message::type::device);
	add_command(doc, device_commands::data);

	rapidjson::Value data;
	data.SetObject();

	make_mandatory_data_info(data, dev, doc.GetAllocator());

	make_full_config_data(data, dev, doc.GetAllocator());
	::Message::add_data(doc, data);
}

std::string device_full_config_to_json(Device const& dev) noexcept
{
	rapidjson::Document doc;
	device_full_config_to_json(doc, dev);

	return ::Message::stringify(doc);
}

void device_board_to_json(rapidjson::Document& doc, Device const& dev) noexcept
{
	doc.SetObject();
	add_type(doc, ::Message::type::device);
	add_command(doc, device_commands::data);

	rapidjson::Value data;
	data.SetObject();

	make_mandatory_data_info(data, dev, doc.GetAllocator());

	make_board_data(data, dev, doc.GetAllocator());
	::Message::add_data(doc, data);
}

std::string device_board_to_json(Device const& dev) noexcept
{
	rapidjson::Document doc;
	device_board_to_json(doc, dev);

	return ::Message::stringify(doc);
}

void device_uptime_to_json(rapidjson::Document& doc, Device const& dev) noexcept
{
	doc.SetObject();
	add_type(doc, ::Message::type::device);
	add_command(doc, device_commands::data);

	rapidjson::Value data;
	data.SetObject();

	make_mandatory_data_info(data, dev, doc.GetAllocator());

	rapidjson::Value v;
	data.AddMember("uptime", ::Message::make_value(v, dev.uptime(), doc.GetAllocator()), doc.GetAllocator());

	::Message::add_data(doc, data);
}

std::string device_uptime_to_json(Device const& dev) noexcept
{
	rapidjson::Document doc;
	device_uptime_to_json(doc, dev);

	return ::Message::stringify(doc);
}

void device_rtc_time_to_json(rapidjson::Document& doc, Device const& dev) noexcept
{
	doc.SetObject();
	add_type(doc, ::Message::type::device);
	add_command(doc, device_commands::data);

	rapidjson::Value data;
	data.SetObject();

	make_mandatory_data_info(data, dev, doc.GetAllocator());

	rapidjson::Value v;
	data.AddMember("rtc", ::Message::make_value(v, dev.rtc_time(), doc.GetAllocator()), doc.GetAllocator());

	::Message::add_data(doc, data);
}

std::string device_rtc_time_to_json(Device const& dev) noexcept
{
	rapidjson::Document doc;
	device_rtc_time_to_json(doc, dev);

	return ::Message::stringify(doc);
}

void device_fuse_to_json(rapidjson::Document& doc, Device const& dev) noexcept
{
	doc.SetObject();
	add_type(doc, ::Message::type::device);
	add_command(doc, device_commands::data);

	rapidjson::Value data;
	data.SetObject();

	make_mandatory_data_info(data, dev, doc.GetAllocator());

	rapidjson::Value v;
	data.AddMember("fuse", dev.fuse(), doc.GetAllocator());

	::Message::add_data(doc, data);
}

std::string device_fuse_to_json(Device const& dev) noexcept
{
	rapidjson::Document doc;
	device_fuse_to_json(doc, dev);

	return ::Message::stringify(doc);

}

void device_jobs_to_json(rapidjson::Document& doc, Device const& dev) noexcept
{
	doc.SetObject();
	add_type(doc, ::Message::type::device);
	add_command(doc, device_commands::data);

	rapidjson::Value data;
	data.SetObject();

	make_mandatory_data_info(data, dev, doc.GetAllocator());

	make_jobs(data, dev, doc.GetAllocator());

	::Message::add_data(doc, data);
}

std::string device_jobs_to_json(Device const& dev) noexcept
{
	rapidjson::Document doc;
	device_jobs_to_json(doc, dev);

	return ::Message::stringify(doc);
}

void device_ota_to_json(rapidjson::Document& doc,
		Device const& dev,
		std::string const& version) noexcept
{
	doc.SetObject();
	add_type(doc, ::Message::type::device);
	add_command(doc, device_commands::data);

	rapidjson::Value data;
	data.SetObject();

	make_mandatory_data_info(data, dev, doc.GetAllocator());

	rapidjson::Value v;
	data.AddMember("ota_version", STRING_TO_JSON_VALUE(version.c_str(), doc), doc.GetAllocator());

	::Message::add_data(doc, data);
}

std::string device_ota_to_json(Device const& dev, std::string const& version) noexcept
{
	rapidjson::Document doc;
	device_ota_to_json(doc, dev, version);

	return ::Message::stringify(doc);
}

void device_apps_to_json(rapidjson::Document& doc, Device const& dev) noexcept
{
	doc.SetObject();
	add_type(doc, ::Message::type::device);
	add_command(doc, device_commands::data);

	rapidjson::Value data;
	data.SetObject();

	make_mandatory_data_info(data, dev, doc.GetAllocator());

	rapidjson::Value v;
	make_apps(data, dev, doc.GetAllocator());

	::Message::add_data(doc, data);
}

std::string device_apps_to_json(Device const& dev) noexcept
{
	rapidjson::Document doc;
	device_apps_to_json(doc, dev);

	return ::Message::stringify(doc);
}

void device_to_json(rapidjson::Document& doc, Device const& dev) noexcept
{
	doc.SetObject();

	add_type(doc, ::Message::type::device);
	add_command(doc, device_commands::data);

	rapidjson::Value data;
	data.SetObject();

	make_mandatory_data_info(data, dev, doc.GetAllocator());

	make_device_data(data, dev, doc.GetAllocator());
	::Message::add_data(doc, data);
}

std::string device_to_json(Device const& dev) noexcept
{
	rapidjson::Document doc;
	device_to_json(doc, dev);

	return ::Message::stringify(doc);
}

void device_list_to_json(rapidjson::Document& doc, Device_List const& list) noexcept
{
	doc.SetObject();

	add_type(doc, ::Message::type::device);
	add_command(doc, device_commands::list);

	rapidjson::Value dev_arr;
	dev_arr.SetArray();

	for(auto const& [addr, dev] : list.list())
	{
		rapidjson::Value devj;
		devj.SetObject();

		make_mandatory_data_info(devj, dev, doc.GetAllocator());
		make_device_data(devj, dev, doc.GetAllocator());
		dev_arr.PushBack(devj, doc.GetAllocator());
	}
	::Message::add_data(doc, dev_arr);
}

std::string device_list_to_json(Device_List const& list) noexcept
{
	rapidjson::Document doc;
	device_list_to_json(doc, list);

	return ::Message::stringify(doc);
}

std::string device_edited_to_json(Device const& dev) noexcept
{
	rapidjson::Document doc;
	doc.SetObject();

	add_type(doc, ::Message::type::device);
	add_command(doc, device_commands::edit);

	rapidjson::Value data;
	data.SetObject();

	::Message::add_device(data, dev.mac(), doc.GetAllocator());
	data.AddMember("name",
			rapidjson::Value(dev.name().data(), dev.name().size(),
						doc.GetAllocator()).Move(),
			doc.GetAllocator());

	::Message::add_data(doc, data);

	return ::Message::stringify(doc);
}

std::string device_reset_reason_to_json(Device const& dev, long reason) noexcept
{
	rapidjson::Document doc;
	doc.SetObject();

	add_type(doc, ::Message::type::device);
	add_command(doc, device_commands::data);

	rapidjson::Value data;
	data.SetObject();

	::Message::add_device(data, dev.mac(), doc.GetAllocator());
	data.AddMember("reset_reason",
					reason,
					doc.GetAllocator());
	data.AddMember("reason_string",
			rapidjson::Value(
					reset_reason_string(static_cast<esp_reset_reason_t>(reason)),
					doc.GetAllocator()).Move(),
			doc.GetAllocator());
	data.AddMember("reason_string_short",
			rapidjson::Value(
					reset_reason_string_short(static_cast<esp_reset_reason_t>(reason)),
					doc.GetAllocator()).Move(),
			doc.GetAllocator());

	::Message::add_data(doc, data);

	return ::Message::stringify(doc);
}

std::string device_tree_to_json(Tree const& tree) noexcept
{
	rapidjson::Document doc;
	doc.SetObject();

	add_type(doc, ::Message::type::device);
	add_command(doc, device_commands::tree);

	rapidjson::Value data;
	data.SetObject();

	auto const unconnected = tree.unconnected();
	rapidjson::Value un;
	un.SetArray();
	for(auto const& u : unconnected)
	{
		auto const& str = u.to_string();
		un.PushBack(rapidjson::Value(str.data(),
				str.size(),
				doc.GetAllocator()).Move(),
				doc.GetAllocator());
	}
	data.AddMember("unconnected", un, doc.GetAllocator());

	rapidjson::Value epsv;
	epsv.SetArray();

	auto const& eps = tree.get_endpoint();
	for(auto const& [addr, ep] : eps)
	{
		rapidjson::Value e;
		e.SetObject();
		::Message::add_endpoint(e, ep.ep, doc.GetAllocator());
		::Message::add_device(e, addr, doc.GetAllocator());
		epsv.PushBack(e, doc.GetAllocator());
	}
	data.AddMember("endpoints", epsv, doc.GetAllocator());

	rapidjson::Value tv;
	tv.SetArray();
	add_node(tv, tree.root(), doc.GetAllocator());
	data.AddMember("tree", tv, doc.GetAllocator());

	::Message::add_data(doc, data);

	return ::Message::stringify(doc);
}

std::string device_custom_response(
		CoAP::Message::message const& request,
		CoAP::Message::message const& response,
		endpoint const& ep,
		CoAP::Transmission::status_t status) noexcept
{
	rapidjson::Document doc;
	doc.SetObject();

	add_type(doc, ::Message::type::device);
	add_command(doc, device_commands::custom_response);

	rapidjson::Value data;
	data.SetObject();

	::Message::add_device(data, request, doc.GetAllocator());
	::Message::add_endpoint(data, ep, doc.GetAllocator());

	::Message::add_request_type(data, request, doc.GetAllocator());

	rapidjson::Value resource;
	::Message::add_option(resource, request, CoAP::Message::Option::code::uri_path, doc.GetAllocator());
	data.AddMember("resource", resource, doc.GetAllocator());

	rapidjson::Value query;
	::Message::add_option(query, request, CoAP::Message::Option::code::uri_query, doc.GetAllocator());
	data.AddMember("query", query, doc.GetAllocator());

	::Message::add_transaction_status(data, status, doc.GetAllocator());
	::Message::add_response_status(data, response.mcode, doc.GetAllocator());

	rapidjson::Value payload;
	::Message::add_payload(payload, response, doc.GetAllocator());
	data.AddMember("payload", payload, doc.GetAllocator());

	::Message::add_data(doc, data);

	return ::Message::stringify(doc);
}

std::string device_sensor_data(Device const& dev,
						Sensor::Sensor_List const& list,
						Sensor::Sensor_Type_List const& type_list) noexcept
{
	rapidjson::Document doc;
	doc.SetObject();

	add_type(doc, ::Message::type::device);
	add_command(doc, device_commands::data);

	rapidjson::Value data;
	data.SetObject();

	make_mandatory_data_info(data, dev, doc.GetAllocator());

	rapidjson::Value sensors;
	Sensor::Message::sensor_list(list, type_list, sensors, doc.GetAllocator());
	data.AddMember("sensor", sensors, doc.GetAllocator());

	::Message::add_data(doc, data);

	return ::Message::stringify(doc);
}

std::string device_sensor_data(Device const& dev,
						const void* sensor_data, std::size_t size,
						Sensor::Sensor_Type_List const& type_list) noexcept
{
	rapidjson::Document doc;
	doc.SetObject();

	add_type(doc, ::Message::type::device);
	add_command(doc, device_commands::data);

	rapidjson::Value data;
	data.SetObject();

	make_mandatory_data_info(data, dev, doc.GetAllocator());

	rapidjson::Value sensors;
	Sensor::Message::sensor_list(sensor_data, size, type_list, sensors, doc.GetAllocator());
	data.AddMember("sensor", sensors, doc.GetAllocator());

	::Message::add_data(doc, data);

	return ::Message::stringify(doc);
}

std::string device_sensor_data(Device const& dev,
						Sensor::sensor_type const& sensor,
						Sensor::Sensor_Type_List const& type_list) noexcept
{
	rapidjson::Document doc;
	doc.SetObject();

	add_type(doc, ::Message::type::device);
	add_command(doc, device_commands::data);

	rapidjson::Value data;
	data.SetObject();

	make_mandatory_data_info(data, dev, doc.GetAllocator());

	rapidjson::Value vsensor, sensors;
	sensors.SetArray();

	Sensor::Message::sensor_value(sensor.value, sensor.type, sensor.index, type_list, vsensor, doc.GetAllocator());
	sensors.PushBack(vsensor, doc.GetAllocator());
	data.AddMember("sensor", sensors, doc.GetAllocator());

	::Message::add_data(doc, data);

	return ::Message::stringify(doc);
}

std::string device_list_sensor_data(Device_List const& list,
								Sensor::Sensor_Type_List const& type_list) noexcept
{
	rapidjson::Document doc;
	doc.SetObject();

	add_type(doc, ::Message::type::device);
	add_command(doc, device_commands::list);

	rapidjson::Value data;
	data.SetArray();

	for(auto const& [key, dev] : list.list())
	{
		rapidjson::Value dev_data;
		dev_data.SetObject();

		::Message::add_device(dev_data, dev.mac(), doc.GetAllocator());

		rapidjson::Value sensors;
		Sensor::Message::sensor_list(dev.sensor_list(), type_list, sensors, doc.GetAllocator());
		dev_data.AddMember("sensor", sensors, doc.GetAllocator());

		data.PushBack(dev_data, doc.GetAllocator());
	}
	::Message::add_data(doc, data);

	return ::Message::stringify(doc);
}

/**
 *
 */
template<typename Allocator>
void add_node(rapidjson::Value& data, Tree::node const& node, Allocator& alloc) noexcept
{
	rapidjson::Value nv;
	nv.SetObject();

	::Message::add_device(nv, node.addr, alloc);
	std::string pt = node.parent ? node.parent->addr.to_string() : "null";
	nv.AddMember("parent",
			rapidjson::Value(pt.data(), pt.size(), alloc).Move(), alloc);
	nv.AddMember("layer", node.layer, alloc);

	rapidjson::Value chv;
	chv.SetArray();
	for(Tree::node const* ch = node.children; ch; ch = ch->next)
	{
		auto const str = ch->addr.to_string();
		chv.PushBack(rapidjson::Value(str.data(), str.size(), alloc).Move(), alloc);
	}
	nv.AddMember("children", chv, alloc);
	data.PushBack(nv, alloc);

	for(Tree::node const* ch = node.children; ch; ch = ch->next)
	{
		add_node(data, *ch, alloc);
	}
}

template<typename Allocator>
static void make_config_data(rapidjson::Value& data, Device const& dev, Allocator& alloc) noexcept
{
	data.AddMember("ch_config", dev.channel_config(), alloc);
	data.AddMember("ch_conn", dev.channel(), alloc);

	char mac_ap[18];
	if(dev.net())
	{
		data.AddMember("net_id",
				rapidjson::Value(dev.net()->net_addr().to_string().data(),
						dev.net()->net_addr().to_string().size(),
				alloc).Move(), alloc);
	}
	data.AddMember("mac_ap", rapidjson::Value(dev.mac_ap().to_string(mac_ap, 18),
			alloc).Move(), alloc);
}

template<typename Allocator>
static void make_route_data(rapidjson::Value& data, Device const& dev, Allocator& alloc) noexcept
{
	char parent[18];
	data.AddMember("parent", rapidjson::Value(dev.parent().to_string(parent, 18),
				alloc).Move(), alloc);
	data.AddMember("layer", dev.layer(), alloc);

	rapidjson::Value child_arr;
	child_arr.SetArray();
	for(auto const& child : dev.children_table())
	{
		char child_buf[18];
		child_arr.PushBack(rapidjson::Value(child.to_string(child_buf, 18),
				alloc).Move(), alloc);
	}
	data.AddMember("children_table", child_arr, alloc);
}

template<typename Allocator>
static void make_full_config_data(rapidjson::Value& data, Device const& dev, Allocator& alloc) noexcept
{
	//Config
	make_config_data(data, dev, alloc);
	//Route
	make_route_data(data, dev, alloc);
}

template<typename Allocator>
static void make_board_data(rapidjson::Value& data, Device const& dev, Allocator& alloc) noexcept
{
	data.AddMember("has_rtc", dev.has_rtc(), alloc);
	data.AddMember("has_temp_sensor", dev.has_temperature_sensor(), alloc);

	data.AddMember("version_fw", rapidjson::Value(dev.firmware_version().c_str(),
					alloc).Move(), alloc);
	data.AddMember("version_hw", rapidjson::Value(dev.hardware_version().c_str(),
					alloc).Move(), alloc);
}

template<typename Allocator>
static void make_job(rapidjson::Value& data, job const& jo, Allocator& alloc) noexcept
{
	data.SetObject();

	rapidjson::Value timei;
	timei.SetObject();
	timei.AddMember("hour", jo.time_before.hour, alloc);
	timei.AddMember("minute", jo.time_before.minute, alloc);

	data.AddMember("time_init", timei, alloc);

	rapidjson::Value timee;
	timee.SetObject();
	timee.AddMember("hour", jo.time_after.hour, alloc);
	timee.AddMember("minute", jo.time_after.minute, alloc);

	data.AddMember("time_end", timee, alloc);

	data.AddMember("dow", jo.dow, alloc);
	data.AddMember("priority", jo.priority, alloc);
	data.AddMember("active", jo.active, alloc);
}

template<typename Allocator>
static void make_jobs(rapidjson::Value& data, Device const& dev, Allocator& alloc) noexcept
{
	auto const& jobs = dev.jobs();
	rapidjson::Value array;
	array.SetArray();
	for(auto const& job : jobs)
	{
		rapidjson::Value j;
		make_job(j, job, alloc);

		array.PushBack(j, alloc);
	}
	data.AddMember("job", array, alloc);
}

template<typename Allocator>
static void make_app(rapidjson::Value& data, app const& app, Allocator& alloc) noexcept
{
	data.SetObject();

	data.AddMember("name", rapidjson::Value(app.name.c_str(), alloc).Move(), alloc);
	data.AddMember("size", app.size, alloc);
}

template<typename Allocator>
static void make_apps(rapidjson::Value& data, Device const& dev, Allocator& alloc) noexcept
{
	auto const& apps = dev.apps();

	rapidjson::Value array;
	array.SetArray();
	for(auto const& app : apps)
	{
		rapidjson::Value j;
		make_app(j, app, alloc);

		array.PushBack(j, alloc);
	}
	data.AddMember("apps", array, alloc);
}

template<typename Allocator>
static void make_others_data(rapidjson::Value& data, Device const& dev, Allocator& alloc) noexcept
{
	data.AddMember("name", rapidjson::Value(dev.name().c_str(), alloc).Move(), alloc);
}

template<typename Allocator>
static void make_device_data(rapidjson::Value& data, Device const& dev, Allocator& alloc) noexcept
{
	//Config
	make_config_data(data, dev, alloc);
	//Route
	make_route_data(data, dev, alloc);
	//Board
	make_board_data(data, dev, alloc);
	//Others
	make_others_data(data, dev, alloc);
}

template<typename Allocator>
static void make_mandatory_data_info(rapidjson::Value& data, Device const& dev, Allocator& alloc) noexcept
{
	data.AddMember("id", dev.id(), alloc);
	::Message::add_device(data, dev.mac(), alloc);
	::Message::add_endpoint(data, dev.get_endpoint(), alloc);
}

static bool add_command(rapidjson::Document& doc, device_commands comm) noexcept
{
	auto const* config = get_config(comm);
	if(!config) return false;

	doc.AddMember("command",
			rapidjson::Value(config->name, doc.GetAllocator()).Move(),
			doc.GetAllocator());

	return true;
}

}//Message
}//Device
}//Agro
