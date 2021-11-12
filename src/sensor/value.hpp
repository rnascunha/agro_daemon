#ifndef AGRO_DAEMON_SENSOR_VALUE_HPP__
#define AGRO_DAEMON_SENSOR_VALUE_HPP__

#include "../device/types.hpp"

namespace Agro{
namespace Sensor{

template<typename Number>
struct Value{
	Value() = default;
	Value(value_time t, Number n);

	value_time		time = 0;
	Number			value = 0;

	void update(Number) noexcept;
	void update(value_time, Number) noexcept;
};

template<typename Number,
		unsigned Max = 20>
class Value_List{
	public:
		using value = Value<std::remove_reference_t<Number>>;
		static constexpr const unsigned max = Max;

		void add(value_time, Number) noexcept;
		void add(Number) noexcept;
		std::size_t size() const noexcept;

		/**
		 * It will add only if a change of the value happened
		 */
		bool add_change(value_time, Number) noexcept;
		bool add_change(Number) noexcept;

		typedef typename std::vector<value>::iterator iterator;
		iterator begin();
		iterator end();

		typedef typename std::vector<value>::const_iterator const_iterator;
		const_iterator begin() const;
		const_iterator end() const;

		value const& operator[](unsigned) const noexcept;
	private:
		std::vector<value> list_;
};

}//Sensor
}//Agro

#include "impl/value_impl.hpp"

#endif /* AGRO_DAEMON_SENSOR_VALUE_HPP__ */
