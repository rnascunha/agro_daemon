#ifndef AGRO_DAEMON_SENSOR_LIST_HPP__
#define AGRO_DAEMON_SENSOR_LIST_HPP__

#include <cstddef>
#include <map>
#include <utility>
#include "value.hpp"
#include "sensor_type.hpp"

namespace Agro{
namespace Sensor{

class Sensor_List{
	public:
		using key = std::pair<unsigned, unsigned>;
		using value = Value_List<int, 20>;
		using container = std::map<key, value>;

//		std::size_t add(const void* data, std::size_t, bool add_change) noexcept;

		bool add(unsigned type, unsigned index,
						sensor_value const& svalue, bool add_change) noexcept;
		bool add(unsigned type, unsigned index,
				value_time time, sensor_value const&, bool add_change) noexcept;
		value& get_or_add(unsigned type, unsigned index) noexcept;

		container::const_iterator begin() const { return list_.begin(); }
		container::const_iterator end() const { return list_.end(); }
		container::const_iterator cbegin() const { return list_.cbegin(); }
		container::const_iterator cend() const { return list_.cend(); }
	private:
		container list_;
};

}//Sensor
}//Agro

#endif /* AGRO_DAEMON_SENSOR_LIST_HPP__ */
