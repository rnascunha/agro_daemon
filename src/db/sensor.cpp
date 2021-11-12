#include "db.hpp"
#include "../sensor/sensor_type.hpp"
#include "tt/tt.hpp"
#include "../helper/time_helper.hpp"

namespace Agro{

int DB::read_sensor_types(Sensor::Sensor_Type_List& list) noexcept
{
	sqlite3::statement res;
	int rc = db_.prepare("SELECT sensor_typeid,name,long_name,type,unit,unit_name,description FROM sensor_type",
						res);
	if(rc != SQLITE_OK)
	{
		return false;
	}

	while(res.step() == SQLITE_ROW)
	{
		std::error_code ec;
		if(ec) continue;

;
		list.add(Sensor::sensor_description{
			res.interger(0),
			res.text(1),
			res.text(2),
			static_cast<Sensor::sensor_unit_type>(res.interger(3)),
			res.text(4),
			res.text(5),
			res.text(6)});
	}

	return true;
}

int DB::add_sensor_type(Sensor::sensor_description const& sensor) noexcept
{
	sqlite3::statement res;
	int rc = db_.prepare_bind("INSERT INTO sensor_type("
			"sensor_typeid,name,long_name,type,description,unit,unit_name) "
			"VALUES(?,?,?,?,?,?,?)",
			res,
			sensor.id,
			sensor.name,
			sensor.long_name,
			static_cast<int>(sensor.type),
			sensor.description,
			sensor.unit,
			sensor.unit_name);

	if(rc != SQLITE_OK)
	{
		return rc;
	}

	rc = res.step();
	return rc;
}

int DB::update_sensor_type(Sensor::sensor_description const& sensor) noexcept
{
	sqlite3::statement res;
	int rc = db_.prepare_bind("UPDATE sensor_type "
			"SET long_name = ?, unit = ?, unit_name = ?, description = ? "
			"WHERE sensor_typeid = ?",
			res,
			sensor.long_name,
			sensor.unit,
			sensor.unit_name,
			sensor.description,
			sensor.id);

	if(rc != SQLITE_OK)
	{
		return rc;
	}

	rc = res.step();
	return rc;
}

int DB::remove_sensor_type(std::string const& name) noexcept
{
	sqlite3::statement res;
	int rc = db_.prepare_bind("DELETE FROM sensor_type WHERE name = ?", res, name);
	if(rc != SQLITE_OK)
	{
		return rc;
	}

	return res.step();

	return SQLITE_OK;
}

/**
 * sensor value
 */

int DB::read_sensor_values(Device::Device& dev) noexcept
{
	sqlite3::statement res;
	int rc = db_.prepare_bind("SELECT DISTINCT sensor_typeid,idx FROM sensor_value WHERE deviceid = ?",
						res, dev.id());
	if(rc != SQLITE_OK)
	{
		tt::debug("Error prepering sensor_type/idx %d", rc);
		return rc;
	}

	std::vector<std::pair<unsigned, unsigned>> list_sensor;
	while(res.step() == SQLITE_ROW)
	{
		list_sensor.push_back(std::make_pair(res.interger(0), res.interger(1)));
	}

	res.clear_bidings();
	for(auto const [type, idx] : list_sensor)
	{
		rc = db_.prepare_bind("SELECT time,value FROM sensor_value "
							"WHERE deviceid = ? AND sensor_typeid = ? AND idx = ? "
							"ORDER BY time DESC "
							"LIMIT ?", res, dev.id(), type, idx, Sensor::Sensor_List::value::max);

		if(rc != SQLITE_OK)
		{
			tt::debug("Error preparing to get sensor_type/idx %d", rc);
			continue;
		}

		std::vector<std::pair<value_time, Sensor::sensor_value>> v;
		while(res.step() == SQLITE_ROW)
		{
			value_time time = res.interger(0);
			int size;
			Sensor::sensor_value s{static_cast<std::uint8_t const*>(res.blob(1, size))};
			v.push_back(std::make_pair(time, s));
		}

		for (std::vector<std::pair<value_time, Sensor::sensor_value>>::reverse_iterator i = v.rbegin();
		        i != v.rend(); ++i )
		{
			dev.update_sensor(type, idx, i->first, i->second);
		}
	}

	return SQLITE_OK;
}

void DB::read_all_sensor_values(Device::Device_List& list) noexcept
{
	for(auto& [addr, dev] : list.list())
	{
		read_sensor_values(dev);
	}
}

std::size_t DB::update_sensor_value(Device::Device const& dev, const void* data, std::size_t size) noexcept
{
	const std::uint8_t* d8u = static_cast<const std::uint8_t*>(data);

	std::size_t dsize = 0, count = 0;

	while((dsize + sizeof(Sensor::sensor_type)) <= size)
	{
		Sensor::sensor_type const* sensor = reinterpret_cast<Sensor::sensor_type const*>(d8u + dsize);
		int rc = update_sensor_value(dev, *sensor);
		if(rc != SQLITE_DONE)
		{
			tt::debug("Error stepping sensor DB statement %d", rc);
		}
		else
		{
			count++;
		}
		dsize += sizeof(Sensor::sensor_type);
	}

	return count;
}

int DB::update_sensor_value(Device::Device const& dev, Sensor::sensor_type const& data) noexcept
{
	sqlite3::statement res;
	int rc = db_.prepare_bind("INSERT INTO sensor_value("
							"deviceid,sensor_typeid,time,value,idx) "
							"VALUES(?,?,?,?,?)", res,
							dev.id(),
							data.type,
							get_time(),
							sqlite3::binary{data.value.array_v, 4},
							data.index);
	if(rc != SQLITE_OK)
	{
		tt::debug("Error preparing sensor DB statement %d", rc);
		return rc;
	}

	return res.step();
}

}//Agro