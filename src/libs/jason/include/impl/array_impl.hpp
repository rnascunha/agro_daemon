#ifndef JASON_ARRAY_IMPL_HPP__
#define JASON_ARRAY_IMPL_HPP__

#include "../jason.hpp"

namespace jason{

template<typename T>
void array_t::push_back(T data) noexcept
{
	assert_base_type<T>();
	value_.PushBack(data, *alloc_);
}

template<typename T, typename... Ts>
void array_t::push_back(T&& t, Ts&&... ts) noexcept
{
	push_back(t);
	push_back(std::forward<Ts>(ts)...);
}

template<typename ...Ts>
void array_t::push_back(Ts&&... ts) noexcept
{
//	jason::push_back(*this, std::forward<Ts>(ts)...);
	push_back(std::forward<Ts>(ts)...);
}

template<typename InputFwIter>
void array_t::push(InputFwIter begin, InputFwIter end) noexcept
{
//	jason::push(*this, begin, end);
	for(InputFwIter i = begin; i != end; ++i)
	{
		push_back(*i);
	}
}

template<typename Container>
void array_t::push(Container const& container) noexcept
{
//	jason::push(*this, container);
	static_assert(is_container<Container>::value, "Is not a contaienr");

	push(container.cbegin(), container.cend());
}

template<typename T>
std::optional<typename T::return_type> array_ref_t::operator[](index<T> const& idx) const noexcept
{
	assert_get_type<T>();

	if(idx.i >= size()) return std::nullopt;
	return get<T>(value_[idx.i]);
}

template<typename T>
std::optional<typename T::return_type> array_t::operator[](index<T> const& idx) const noexcept
{
	assert_get_type<T>();

	if(idx.i >= size()) return std::nullopt;
	return jason::get<T>(value_[idx.i]);
}

template<typename T>
array_t& array_t::operator<<(T t) noexcept
{
	push_back(t);

	return *this;
}

}//jason

#endif /* JASON_ARRAY_IMPL_HPP__ */
