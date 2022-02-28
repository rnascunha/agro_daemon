#ifndef JASON_ARRAY_IMPL_HPP__
#define JASON_ARRAY_IMPL_HPP__

#include "../jason.hpp"

namespace jason{


template<typename T>
void push_back(array_t& value, T data) noexcept
{
	assert_base_type<T>();
	value.native().PushBack(data, value.allocator());
}

template<typename T, typename... Ts>
void push_back(array_t& arr, T&& t, Ts&&... ts) noexcept
{
	push_back(arr, t);
	push_back(arr, std::forward<Ts>(ts)...);
}

template<typename... Ts>
void push_back(array_t& arr, Ts&&... ts) noexcept
{
	push_back(arr, std::forward<Ts>(ts)...);
}

template<typename InputFwIter>
void push(array_t& arr, InputFwIter begin, InputFwIter end) noexcept
{
	for(InputFwIter i = begin; i != end; ++i)
	{
		push_back(arr, *i);
	}
}

template<typename Container>
void push(array_t& arr, Container const& container) noexcept
{
	static_assert(is_container<Container>::value, "Is not a contaienr");

	push(arr, container.cbegin(), container.cend());
}

template<typename ...Ts>
void array_t::push_back(Ts&&... ts) noexcept
{
	jason::push_back(*this, std::forward<Ts>(ts)...);
}

template<typename InputIter>
void array_t::push(InputIter begin, InputIter end) noexcept
{
	jason::push(*this, begin, end);
}

template<typename Container>
void array_t::push(Container const& container) noexcept
{
	jason::push(*this, container);
}

template<typename T>
std::optional<typename T::return_type> array_ref_t::operator[](index<T> const& idx) const noexcept
{
	assert_get_type<T>();

	if(idx.i >= size()) return std::nullopt;
	return get<T>(value[idx.i]);
}

template<typename T>
std::optional<typename T::return_type> array_t::operator[](index<T> const& idx) const noexcept
{
	assert_get_type<T>();

	if(idx.i >= size()) return std::nullopt;
	return jason::get<T>(value[idx.i]);
}

template<typename T>
array_t& array_t::operator<<(T t) noexcept
{
	push_back(t);
	return *this;
}

}//jason

#endif /* JASON_ARRAY_IMPL_HPP__ */
