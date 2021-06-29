#ifndef AGRO_MESH_DEVICE_TYPES_IMPL_HPP__
#define AGRO_MESH_DEVICE_TYPES_IMPL_HPP__

#include "../types.hpp"
#include <chrono>

template<typename Number,
		unsigned Max>
void Value_List<Number, Max>::add(value_time time, Number value) noexcept
{
	list_.emplace_back(time, value);
	if(list_.size() > Max)
	{
		list_.erase(list_.begin());
	}
}

template<typename Number,
		unsigned Max>
void Value_List<Number, Max>::add(Number value) noexcept
{
	list_.emplace_back(static_cast<value_time>(std::chrono::duration_cast<std::chrono::seconds>(
							std::chrono::system_clock::now().time_since_epoch()).count()),
					std::remove_reference_t<Number>{value});
	if(list_.size() > Max)
	{
		list_.erase(list_.begin());
	}
}

template<typename Number,
		unsigned Max>
std::size_t Value_List<Number, Max>::size() const noexcept
{
	return list_.size();
}

template<typename Number,
		unsigned Max>
typename Value_List<Number, Max>::iterator Value_List<Number, Max>::begin()
{
	return list_.begin();
}

template<typename Number,
		unsigned Max>
typename Value_List<Number, Max>::iterator Value_List<Number, Max>::end()
{
	return list_.end();
}

template<typename Number,
		unsigned Max>
typename Value_List<Number, Max>::const_iterator Value_List<Number, Max>::cbegin() const
{
	return list_.cbegin();
}

template<typename Number,
		unsigned Max>
typename Value_List<Number, Max>::const_iterator Value_List<Number, Max>::cend() const
{
	return list_.cend();
}

template<typename Number,
		unsigned Max>
typename Value_List<Number, Max>::value const& Value_List<Number, Max>::operator[](unsigned index) const
{
	return list_[index];
}

#endif /* AGRO_MESH_DEVICE_TYPES_IMPL_HPP__ */

