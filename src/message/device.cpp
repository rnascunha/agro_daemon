#include "device.hpp"
#include "make.hpp"

namespace Message{

void device_to_json(rapidjson::Document& doc, Device const& dev) noexcept
{
	doc.SetObject();

	add_endpoint(doc, dev.get_endpoint());
	add_device(doc, dev.mac());

	rapidjson::Value data;

	data.SetObject();

	//Config
	data.AddMember("ch_config", dev.channel_config(), doc.GetAllocator());
	data.AddMember("ch_conn", dev.channel(), doc.GetAllocator());

	char net_id[18], mac_ap[18];
	data.AddMember("net_id", rapidjson::Value(dev.net_id().to_string(net_id, 18),
			doc.GetAllocator()).Move(), doc.GetAllocator());
	data.AddMember("mac_ap", rapidjson::Value(dev.mac_ap().to_string(mac_ap, 18),
			doc.GetAllocator()).Move(), doc.GetAllocator());

	//Status
	rapidjson::Value arr_rssi;
	arr_rssi.SetArray();
	auto const& rrssi = dev.rssi();
//	for(auto const& r : rrssi)
	for(std::size_t i = 0; i < rrssi.size(); i++)
	{
		rapidjson::Value v;
		v.SetObject();
		v.AddMember("time", rrssi[i].time, doc.GetAllocator());
		v.AddMember("value", rrssi[i].value, doc.GetAllocator());

		arr_rssi.PushBack(v, doc.GetAllocator());
	}
	data.AddMember("rssi", arr_rssi, doc.GetAllocator());

	//Route
	char parent[18];
	data.AddMember("parent", rapidjson::Value(dev.parent().to_string(parent, 18),
				doc.GetAllocator()).Move(), doc.GetAllocator());
	data.AddMember("layer", htons(dev.layer()), doc.GetAllocator());

	rapidjson::Value child_arr;
	child_arr.SetArray();
	for(auto const& child : dev.children_table())
	{
		char child_buf[18];
		child_arr.PushBack(rapidjson::Value(child.to_string(child_buf, 18),
				doc.GetAllocator()).Move(), doc.GetAllocator());
	}
	data.AddMember("children_table", child_arr, doc.GetAllocator());

	rapidjson::Value child_arr2;
	child_arr.SetArray();
	for(auto const& child : dev.children())
	{
		char child_buf[18];
		child_arr.PushBack(rapidjson::Value(child.to_string(child_buf, 18),
				doc.GetAllocator()).Move(), doc.GetAllocator());
	}
	data.AddMember("children", child_arr, doc.GetAllocator());

	//Board
	data.AddMember("has_rtc", dev.has_rtc(), doc.GetAllocator());
	data.AddMember("has_temp_sensor", dev.has_temperature_sensor(), doc.GetAllocator());

	data.AddMember("version_fw", rapidjson::Value(dev.firmware_version().c_str(),
					doc.GetAllocator()).Move(), doc.GetAllocator());
	data.AddMember("version_hw", rapidjson::Value(dev.hardware_version().c_str(),
					doc.GetAllocator()).Move(), doc.GetAllocator());

	add_data(doc, data);
}

std::string device_to_json(Device const& dev) noexcept
{
	rapidjson::Document doc;
	device_to_json(doc, dev);

	return stringify(doc);
}

}//Message
