#ifndef JASON_OBJECT_HPP__
#define JASON_OBJECT_HPP__

#include "../jason.hpp"

namespace jason{

template<typename T>
struct is_type_object : has_type<std::remove_cv_t<std::remove_reference_t<T>>, object_t, object_ref_t>{};

/**
 * Set
 */
template<typename T>
void object_t::set(key_type key, T data) noexcept
{
	assert_base_type<T>();
	value_.AddMember(rapidjson::StringRef(key), data, *alloc_);
}

}//jason

#endif /* JASON_OBJECT_HPP__ */
