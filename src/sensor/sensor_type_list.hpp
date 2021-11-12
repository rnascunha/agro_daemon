#ifndef AGRO_DAEMON_SENSOR_TYPE_LIST_HPP__
#define AGRO_DAEMON_SENSOR_TYPE_LIST_HPP__

#include <string>
#include <unordered_map>

namespace Agro{
namespace Sensor{

enum class sensor_unit_type{
	integer = 0,
	tunsigned,
	tfloat,
	array
};

using sensor_id = int;

struct sensor_description{
	sensor_id	id;
	std::string name;
	std::string long_name;
	sensor_unit_type type;
	std::string unit;
	std::string unit_name;
	std::string description;
};

class Sensor_Type_List{
	public:
		sensor_description* add(sensor_description const&) noexcept;
		bool update(sensor_description const&) noexcept;
		bool remove(std::string const&) noexcept;
		bool remove(sensor_id) noexcept;

		sensor_description const* get(sensor_id) const noexcept;

		std::unordered_map<sensor_id, sensor_description>::iterator begin() { return list_.begin(); }
		std::unordered_map<sensor_id, sensor_description>::iterator end() { return list_.end(); }

		std::unordered_map<sensor_id, sensor_description>::const_iterator begin() const { return list_.begin(); }
		std::unordered_map<sensor_id, sensor_description>::const_iterator end() const { return list_.end(); }
		std::unordered_map<sensor_id, sensor_description>::const_iterator cbegin() const { return list_.cbegin(); }
		std::unordered_map<sensor_id, sensor_description>::const_iterator cend() const { return list_.cend(); }
	private:
		std::unordered_map<sensor_id, sensor_description> list_;
};

}//Sensor
}//Agro

#endif /* AGRO_DAEMON_SENSOR_TYPE_LIST_HPP__ */
