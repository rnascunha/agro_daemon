#ifndef JASON_OBJECT_HPP__
#define JASON_OBJECT_HPP__

#include "../jason.hpp"

namespace jason{

/**
 * Set
 */
template<typename T>
void set(object_t& value, key_type key, T data) noexcept
{
	assert_base_type<T>();
	value.native().AddMember(rapidjson::StringRef(key), data, value.allocator());
}

template<typename T>
void object_t::set(key_type key, T&& value) noexcept
{
	jason::set(*this, key, std::forward<T>(value));
}

}//jason

#endif /* JASON_OBJECT_HPP__ */
