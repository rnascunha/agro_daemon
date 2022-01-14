#include "db.hpp"
#include "../coap_engine.hpp"

namespace Agro{

bool DB::read_devices_net(Device::Net_List& net_list) noexcept
{
	sqlite3::statement res;
	int rc = db_.prepare("SELECT netid,net_addr,name FROM net", res);
	if(rc != SQLITE_OK)
	{
		return false;
	}

	while(res.step() == SQLITE_ROW)
	{
		std::error_code ec;
		auto str = res.text(1);
		mesh_addr_t addr(str.data(), str.size(),ec);
		if(ec) continue;

		net_list.add(Device::Net{res.integer(0), addr, res.text(2)});
	}

	return true;
}

bool DB::read_devices(Device::Device_List& device_list, Device::Net_List const& net_list) noexcept
{
	sqlite3::statement res;
	int rc = db_.prepare("SELECT deviceid,mac,mac_ap,parent_mac,"
								"endpoint_addr,endpoint_port,name,"
								"fw_version,hw_version,"
								"channel_config,channel,"
								"has_rtc,has_temp_sensor,"
								"layer,netid,fuse "
								"FROM device", res);
	if(rc != SQLITE_OK)
	{
		return false;
	}

	while(res.step() == SQLITE_ROW)
	{
		std::error_code ec;
		auto str = res.text(1);
		mesh_addr_t addr(str.data(), str.size(),ec);
		if(ec) continue;

		auto mac_ap_str = res.text(2);
		mesh_addr_t mac_ap(mac_ap_str.data(), mac_ap_str.size(), ec);
		if(ec) continue;

		auto parent_str = res.text(3);
		mesh_addr_t parent(parent_str.data(), parent_str.size(), ec);
		if(ec) continue;

		CoAP::Error ecp;
		endpoint ep(res.text(4).c_str(), static_cast<std::uint16_t>(res.integer(5)), ecp);
		if(ecp) continue;

		auto* dev = device_list.add(Device::Device{res.integer(0),
									addr, mac_ap, parent, ep,
									res.text(6),	//name
									res.text(7),	//fw
									res.text(8),	//hw
									static_cast<uint8_t>(res.integer(9)), //channel config
									static_cast<uint8_t>(res.integer(10)), //channel
									res.integer(11) ? true : false, //has_rtc
									res.integer(12) ? true : false, //has_temp_sensor
									res.integer(13)	//layer
									});
		dev->net(net_list.get(res.integer(14)));
		dev->fuse(res.integer(15));

		sqlite3::statement res2;
		rc = db_.prepare_bind("SELECT children_mac FROM children_table WHERE deviceid = ?",
				res2, dev->id());
		if(rc != SQLITE_OK) continue;

		dev->clear_children();
		while(res2.step() == SQLITE_ROW)
		{
			auto ch = res.text(0);
			mesh_addr_t addr_c{ch.data(), static_cast<unsigned>(ch.size()), ec};
			dev->add_children(addr_c);
		}
	}

	return true;
}

int DB::add_device(mesh_addr_t const& addr, Device::device_id& id) noexcept
{
	id = -1;
	sqlite3::statement res;
	std::string addr_str = addr.to_string();
	int rc = db_.prepare_bind("INSERT INTO device(mac) VALUES(?)",
				res, addr_str);
	if(rc != SQLITE_OK)
	{
		return rc;
	}

	rc = res.step();
	if(rc == SQLITE_DONE)
	{
		id = db_.last_insert_rowid();
	}

	return rc;
}

int DB::add_net(mesh_addr_t const& net_addr,
		Device::net_id& id, std::string const& name/* = "" */) noexcept
{
	id = -1;
	std::string net_addr_str = net_addr.to_string();

	sqlite3::statement res;
	int rc = db_.prepare_bind("INSERT INTO net(net_addr, name) VALUES(?,?)",
				res, net_addr_str, name);
	if(rc != SQLITE_OK)
	{
		return rc;
	}

	rc = res.step();
	if(rc == SQLITE_DONE)
	{
		id = db_.last_insert_rowid();
	}

	return rc;
}

int DB::update_device(Device::Device const& device) noexcept
{
	char addr[INET_ADDRSTRLEN];
	device.get_endpoint().address(addr, INET_ADDRSTRLEN);
	std::string mac_ap_str = device.mac_ap().to_string(),
				parent_mac = device.parent().to_string();

	sqlite3::statement res;
	int rc = db_.prepare_bind(
			"UPDATE device "
			"SET name = ?, fw_version = ?,hw_version = ?,"
			"mac_ap = ?,fuse = ?,channel = ?,channel_config = ?,"
			"endpoint_addr = ?,endpoint_port = ?,layer = ?,"
			"netid = ?,parent_mac = ?,"
			"has_rtc = ?,has_temp_sensor = ?"
			"WHERE deviceid = ?", res,
				device.name(), device.firmware_version(),
				device.hardware_version(), mac_ap_str,
				device.fuse(), device.channel(), device.channel_config(),
				addr, device.get_endpoint().port(), device.layer(),
				device.net() ? device.net()->id() : 0, parent_mac,
				device.has_rtc(), device.has_temperature_sensor(),
				device.id());
	if(rc != SQLITE_OK)
	{
		return rc;
	}

	return res.step();
}

int DB::update_device_route(Device::Device const& device) noexcept
{
	std::string parent_mac = device.parent().to_string();

	sqlite3::statement res;
	int rc = db_.prepare_bind(
			"UPDATE device SET "
			"layer = ?,parent_mac = ? "
			"WHERE deviceid = ?", res,
				device.layer(), parent_mac, device.id());
	if(rc != SQLITE_OK)
	{
		return rc;
	}

	rc = res.step();
	if(rc != SQLITE_DONE)
	{
		return rc;
	}
	res.finalize();

	rc = db_.prepare_bind("DELETE FROM children_table WHERE deviceid = ?",
					res, device.id());
	if(rc != SQLITE_OK)
	{
		return rc;
	}

	rc = res.step();
	if(rc != SQLITE_DONE)
	{
		return rc;
	}
	res.finalize();

	rc = db_.prepare("INSERT INTO children_table(deviceid,children_mac) VALUES(?,?)", res);
	for(auto const& child : device.children_table())
	{
		res.clear_bidings();

		res.bind(1, device.id());
		std::string c = child.to_string();
		res.bind(2, c);

		res.step();
	}

	return SQLITE_DONE;
}

int DB::set_device_state(Device::Device const& device, bool connected) noexcept
{
	sqlite3::statement res;
	int rc = db_.prepare_bind("INSERT INTO device_state(deviceid,time,connected) VALUES(?,?,?)",
				res, device.id(), get_time(), connected);
	if(rc != SQLITE_OK)
	{
		return rc;
	}

	return res.step();
}
}//Agro
