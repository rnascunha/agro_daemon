#include "device.hpp"
#include "make.hpp"

namespace Message{

template<typename Allocator>
static void make_config_data(rapidjson::Value& data, Device const& dev, Allocator& alloc) noexcept;
template<typename Allocator>
static void make_status_data(rapidjson::Value& data, Device const& dev, Allocator& alloc) noexcept;
template<typename Allocator>
static void make_route_data(rapidjson::Value& data, Device const& dev, Allocator& alloc) noexcept;
template<typename Allocator>
static void make_full_config_data(rapidjson::Value& data, Device const& dev, Allocator& alloc) noexcept;
template<typename Allocator>
static void make_board_data(rapidjson::Value& data, Device const& dev, Allocator& alloc) noexcept;
template<typename Allocator>
static void make_others_data(rapidjson::Value& data, Device const& dev, Allocator& alloc) noexcept;
template<typename Allocator>
static void make_sensor_data(rapidjson::Value& data, Device const& dev, Allocator& alloc) noexcept;
template<typename Allocator>
static void make_gpios(rapidjson::Value& data, Device const& dev, Allocator& alloc) noexcept;
template<typename Allocator>
static void make_job(rapidjson::Value& data, job const&, Allocator& alloc) noexcept;
template<typename Allocator>
static void make_jobs(rapidjson::Value& data, Device const& dev, Allocator& alloc) noexcept;
template<typename Allocator>
static void make_apps(rapidjson::Value& data, Device const& dev, Allocator& alloc) noexcept;
template<typename Allocator>
static void make_device_data(rapidjson::Value& data, Device const& dev, Allocator& alloc) noexcept;

void device_config_to_json(rapidjson::Document& doc, Device const& dev) noexcept
{
	doc.SetObject();
	add_type(doc, type::device);
	add_device(doc, dev.mac());

	rapidjson::Value data;
	data.SetObject();
	add_endpoint(data, dev.get_endpoint(), doc.GetAllocator());

	make_config_data(data, dev, doc.GetAllocator());
	add_data(doc, data);
}

std::string device_config_to_json(Device const& dev) noexcept
{
	rapidjson::Document doc;
	device_config_to_json(doc, dev);

	return std::string{stringify(doc)};
}

void device_status_to_json(rapidjson::Document& doc, Device const& dev) noexcept
{
	doc.SetObject();
	add_type(doc, type::device);
	add_device(doc, dev.mac());

	rapidjson::Value data;
	data.SetObject();
	add_endpoint(data, dev.get_endpoint(), doc.GetAllocator());

	make_status_data(data, dev, doc.GetAllocator());
	add_data(doc, data);
}

std::string device_status_to_json(Device const& dev) noexcept
{
	rapidjson::Document doc;
	device_status_to_json(doc, dev);

	return std::string{stringify(doc)};
}

void device_route_to_json(rapidjson::Document& doc, Device const& dev) noexcept
{
	doc.SetObject();
	add_type(doc, type::device);
	add_device(doc, dev.mac());

	rapidjson::Value data;
	data.SetObject();
	add_endpoint(data, dev.get_endpoint(), doc.GetAllocator());

	make_route_data(data, dev, doc.GetAllocator());
	add_data(doc, data);
}

std::string device_route_to_json(Device const& dev) noexcept
{
	rapidjson::Document doc;
	device_route_to_json(doc, dev);

	return std::string{stringify(doc)};
}

void device_full_config_to_json(rapidjson::Document& doc, Device const& dev) noexcept
{
	doc.SetObject();
	add_type(doc, type::device);
	add_device(doc, dev.mac());

	rapidjson::Value data;
	data.SetObject();
	add_endpoint(data, dev.get_endpoint(), doc.GetAllocator());

	make_full_config_data(data, dev, doc.GetAllocator());
	add_data(doc, data);
}

std::string device_full_config_to_json(Device const& dev) noexcept
{
	rapidjson::Document doc;
	device_full_config_to_json(doc, dev);

	return std::string{stringify(doc)};
}

void device_board_to_json(rapidjson::Document& doc, Device const& dev) noexcept
{
	doc.SetObject();
	add_type(doc, type::device);
	add_device(doc, dev.mac());

	rapidjson::Value data;
	data.SetObject();
	add_endpoint(data, dev.get_endpoint(), doc.GetAllocator());

	make_board_data(data, dev, doc.GetAllocator());
	add_data(doc, data);
}

std::string device_board_to_json(Device const& dev) noexcept
{
	rapidjson::Document doc;
	device_board_to_json(doc, dev);

	return std::string{stringify(doc)};
}

void device_sensor_data_to_json(rapidjson::Document& doc, Device const& dev) noexcept
{
	doc.SetObject();
	add_type(doc, type::device);
	add_device(doc, dev.mac());

	rapidjson::Value data;
	data.SetObject();
	add_endpoint(data, dev.get_endpoint(), doc.GetAllocator());

	make_sensor_data(data, dev, doc.GetAllocator());
	add_data(doc, data);
}

std::string device_sensor_data_to_json(Device const& dev) noexcept
{
	rapidjson::Document doc;
	device_sensor_data_to_json(doc, dev);

	return std::string{stringify(doc)};
}

void device_gpios_to_json(rapidjson::Document& doc, Device const& dev) noexcept
{
	doc.SetObject();
	add_type(doc, type::device);
	add_device(doc, dev.mac());

	rapidjson::Value data;
	data.SetObject();
	add_endpoint(data, dev.get_endpoint(), doc.GetAllocator());

	make_gpios(data, dev, doc.GetAllocator());
	add_data(doc, data);
}

std::string device_gpios_to_json(Device const& dev) noexcept
{
	rapidjson::Document doc;
	device_gpios_to_json(doc, dev);

	return std::string{stringify(doc)};
}

void device_uptime_to_json(rapidjson::Document& doc, Device const& dev) noexcept
{
	doc.SetObject();
	add_type(doc, type::device);
	add_device(doc, dev.mac());

	rapidjson::Value data;
	data.SetObject();
	add_endpoint(data, dev.get_endpoint(), doc.GetAllocator());
	rapidjson::Value v;
	data.AddMember("uptime", make_value(v, dev.uptime(), doc.GetAllocator()), doc.GetAllocator());

	add_data(doc, data);
}

std::string device_uptime_to_json(Device const& dev) noexcept
{
	rapidjson::Document doc;
	device_uptime_to_json(doc, dev);

	return std::string{stringify(doc)};
}

void device_rtc_time_to_json(rapidjson::Document& doc, Device const& dev) noexcept
{
	doc.SetObject();
	add_type(doc, type::device);
	add_device(doc, dev.mac());

	rapidjson::Value data;
	data.SetObject();
	add_endpoint(data, dev.get_endpoint(), doc.GetAllocator());
	rapidjson::Value v;
	data.AddMember("rtc", make_value(v, dev.rtc_time(), doc.GetAllocator()), doc.GetAllocator());

	add_data(doc, data);
}

std::string device_rtc_time_to_json(Device const& dev) noexcept
{
	rapidjson::Document doc;
	device_rtc_time_to_json(doc, dev);

	return std::string{stringify(doc)};
}

void device_fuse_to_json(rapidjson::Document& doc, Device const& dev) noexcept
{
	doc.SetObject();
	add_type(doc, type::device);
	add_device(doc, dev.mac());

	rapidjson::Value data;
	data.SetObject();
	add_endpoint(data, dev.get_endpoint(), doc.GetAllocator());
	rapidjson::Value v;
	data.AddMember("fuse", dev.fuse(), doc.GetAllocator());

	add_data(doc, data);
}

std::string device_fuse_to_json(Device const& dev) noexcept
{
	rapidjson::Document doc;
	device_fuse_to_json(doc, dev);

	return std::string{stringify(doc)};

}

void device_jobs_to_json(rapidjson::Document& doc, Device const& dev) noexcept
{
	doc.SetObject();
	add_type(doc, type::device);
	add_device(doc, dev.mac());

	rapidjson::Value data;
	data.SetObject();
	add_endpoint(data, dev.get_endpoint(), doc.GetAllocator());
	make_jobs(data, dev, doc.GetAllocator());

	add_data(doc, data);
}

std::string device_jobs_to_json(Device const& dev) noexcept
{
	rapidjson::Document doc;
	device_jobs_to_json(doc, dev);

	return std::string{stringify(doc)};
}

void device_ota_to_json(rapidjson::Document& doc,
		Device const& dev,
		std::string const& version) noexcept
{
	doc.SetObject();
	add_type(doc, type::device);
	add_device(doc, dev.mac());

	rapidjson::Value data;
	data.SetObject();
	add_endpoint(data, dev.get_endpoint(), doc.GetAllocator());
	rapidjson::Value v;
	data.AddMember("ota_version", STRING_TO_JSON_VALUE(version.c_str(), doc), doc.GetAllocator());

	add_data(doc, data);
}

std::string device_ota_to_json(Device const& dev, std::string const& version) noexcept
{
	rapidjson::Document doc;
	device_ota_to_json(doc, dev, version);

	return std::string{stringify(doc)};
}

void device_apps_to_json(rapidjson::Document& doc, Device const& dev) noexcept
{
	doc.SetObject();
	add_type(doc, type::device);
	add_device(doc, dev.mac());

	rapidjson::Value data;
	data.SetObject();
	add_endpoint(data, dev.get_endpoint(), doc.GetAllocator());
	rapidjson::Value v;
	make_apps(data, dev, doc.GetAllocator());

	add_data(doc, data);
}

std::string device_apps_to_json(Device const& dev) noexcept
{
	rapidjson::Document doc;
	device_apps_to_json(doc, dev);

	return std::string{stringify(doc)};
}

void device_to_json(rapidjson::Document& doc, Device const& dev) noexcept
{
	doc.SetObject();

	add_type(doc, type::device);
	add_device(doc, dev.mac());

	rapidjson::Value data;
	data.SetObject();

	make_device_data(data, dev, doc.GetAllocator());
	add_data(doc, data);
}

std::string device_to_json(Device const& dev) noexcept
{
	rapidjson::Document doc;
	device_to_json(doc, dev);

	return stringify(doc);
}

void device_list_to_json(rapidjson::Document& doc, Device_List const& list) noexcept
{
	doc.SetObject();

	add_type(doc, type::device_list);
	rapidjson::Value dev_arr;
	dev_arr.SetArray();

	for(auto const& [addr, dev] : list.list())
	{
		rapidjson::Value devj;
		devj.SetObject();

		make_device_data(devj, dev, doc.GetAllocator());
		add_device(devj, dev.mac(), doc.GetAllocator());

		dev_arr.PushBack(devj, doc.GetAllocator());
	}
	add_data(doc, dev_arr);
}

std::string device_list_to_json(Device_List const& list) noexcept
{
	rapidjson::Document doc;
	device_list_to_json(doc, list);

	return stringify(doc);
}

/**
 *
 */
template<typename Allocator>
static void make_config_data(rapidjson::Value& data, Device const& dev, Allocator& alloc) noexcept
{
	data.AddMember("ch_config", dev.channel_config(), alloc);
	data.AddMember("ch_conn", dev.channel(), alloc);

	char net_id[18], mac_ap[18];
	data.AddMember("net_id", rapidjson::Value(dev.net_id().to_string(net_id, 18),
			alloc).Move(), alloc);
	data.AddMember("mac_ap", rapidjson::Value(dev.mac_ap().to_string(mac_ap, 18),
			alloc).Move(), alloc);
}

template<typename Allocator>
static void make_status_data(rapidjson::Value& data, Device const& dev, Allocator& alloc) noexcept
{
	//Status
	rapidjson::Value arr_rssi;
	arr_rssi.SetArray();
	auto const& rrssi = dev.rssi();
	for(auto const& r : rrssi)
	{
		rapidjson::Value v;
		v.SetObject();
		v.AddMember("time", r.time, alloc);
		v.AddMember("value", r.value, alloc);

		arr_rssi.PushBack(v, alloc);
	}
	data.AddMember("rssi", arr_rssi, alloc);
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

	rapidjson::Value child_arr2;
	child_arr.SetArray();
	for(auto const& child : dev.children())
	{
		char child_buf[18];
		child_arr.PushBack(rapidjson::Value(child.to_string(child_buf, 18),
				alloc).Move(), alloc);
	}
	data.AddMember("children", child_arr, alloc);
}

template<typename Allocator>
static void make_full_config_data(rapidjson::Value& data, Device const& dev, Allocator& alloc) noexcept
{
	//Config
	make_config_data(data, dev, alloc);
	//Status
	make_status_data(data, dev, alloc);
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
static void make_gpios(rapidjson::Value& data, Device const& dev, Allocator& alloc) noexcept
{
	rapidjson::Value gpios_v;
	make_value_list_array(gpios_v, dev.gpios(), alloc);
	data.AddMember("gpios", gpios_v, alloc);
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
static void make_sensor_data(rapidjson::Value& data, Device const& dev, Allocator& alloc) noexcept
{
	data.AddMember("time", dev.last_packet_time(), alloc);

	rapidjson::Value temp_v;
	make_value_list_array(temp_v, dev.temperature(), alloc);
	data.AddMember("temp", temp_v, alloc);

	rapidjson::Value rssi_v;
	make_value_list_array(rssi_v, dev.rssi(), alloc);
	data.AddMember("rssi", rssi_v, alloc);

	rapidjson::Value gpios_v;
	make_value_list_array(gpios_v, dev.gpios(), alloc);
	data.AddMember("gpios", gpios_v, alloc);
}

template<typename Allocator>
static void make_others_data(rapidjson::Value& data, Device const& dev, Allocator& alloc) noexcept
{
	data.AddMember("name", rapidjson::Value(dev.name().c_str(), alloc).Move(), alloc);
}

template<typename Allocator>
static void make_device_data(rapidjson::Value& data, Device const& dev, Allocator& alloc) noexcept
{
	add_endpoint(data, dev.get_endpoint(), alloc);
	//Config
	make_config_data(data, dev, alloc);
	//Status
//	make_status_data(data, dev, alloc);
	make_sensor_data(data, dev, alloc);
	//Route
	make_route_data(data, dev, alloc);
	//Board
	make_board_data(data, dev, alloc);
	//Others
	make_others_data(data, dev, alloc);
}

}//Message
