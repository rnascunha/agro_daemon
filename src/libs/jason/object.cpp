#include "jason.hpp"

namespace jason{

bool object_t::is(json_type const& value) noexcept
{
	return value.IsObject();
}

object_t::return_type object_t::get(json_type const& value) noexcept
{
	return object_t::return_type{value.GetObject()};
}

bool object_ref_t::has(key_type key) const noexcept
{
	return value_.HasMember(key);
}

bool object_t::has(key_type key) const noexcept
{
	return value_.HasMember(key);
}

json_type const& object_ref_t::operator[](key_type key) const noexcept
{
	return value_[key];
}

template<typename T>
std::optional<typename T::return_type> object_ref_t::operator[](field<T> const& key) const noexcept
{
	return get(value_, key);
}

json_type& object_t::operator[](key_type key) noexcept
{
	return value_[key];
}

json_type const& object_t::operator[](key_type key) const noexcept
{
	return value_[key];
}

template<typename T>
std::optional<typename T::return_type> object_t::operator[](field<T> const& key) const noexcept
{
	return get(*this, key);
}

void object_t::set(key_type key, const char* str) noexcept
{
	value_.AddMember(rapidjson::StringRef(key),
				rapidjson::StringRef(str),
				*alloc_);
}

void object_t::set(key_type key, const char* str, std::size_t size) noexcept
{
	value_.AddMember(rapidjson::StringRef(key),
					rapidjson::StringRef(str, size),
					*alloc_);
}

void object_t::set(key_type key, std::string const& str) noexcept
{
	set(key, str.data(), str.size());
}

void object_t::set(key_type key, object_t& data) noexcept
{
	value_.AddMember(rapidjson::StringRef(key), data.native(), *alloc_);
}

void object_t::set(key_type key, array_t& data) noexcept
{
	value_.AddMember(rapidjson::StringRef(key), data.native(), *alloc_);
}

}//jason

