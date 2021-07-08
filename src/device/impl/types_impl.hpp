#ifndef AGRO_MESH_DEVICE_TYPES_IMPL_HPP__
#define AGRO_MESH_DEVICE_TYPES_IMPL_HPP__

#include "../types.hpp"

template<typename Number>
Value<Number>::Value(value_time t, Number n)
	: time(t), value(n){}

template<typename Number>
void Value<Number>::update(Number val) noexcept
{
	time = get_time();
	value = val;
}

template<typename Number>
void Value<Number>::update(value_time t, Number val) noexcept
{
	time = t;
	value = val;
}

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
	list_.emplace_back(get_time(), std::remove_reference_t<Number>{value});
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
bool Value_List<Number, Max>::add_change(value_time time, Number value) noexcept
{
	if(list_.size() == 0)
	{
		add(time, value);
		return true;
	}

	Number v = list_.back().value;
	if(v == value) return false;

	add(time, value);
	return true;
}

template<typename Number,
		unsigned Max>
bool Value_List<Number, Max>::add_change(Number value) noexcept
{
	if(list_.size() == 0)
	{
		add(value);
		return true;
	}

	Number v = list_.back().value;
	if(v == value) return false;

	add(value);
	return true;
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
typename Value_List<Number, Max>::const_iterator Value_List<Number, Max>::begin() const
{
	return list_.cbegin();
}

template<typename Number,
		unsigned Max>
typename Value_List<Number, Max>::const_iterator Value_List<Number, Max>::end() const
{
	return list_.cend();
}

template<typename Number,
		unsigned Max>
typename Value_List<Number, Max>::value const& Value_List<Number, Max>::operator[](unsigned index) const noexcept
{
	return list_[index];
}

#endif /* AGRO_MESH_DEVICE_TYPES_IMPL_HPP__ */

