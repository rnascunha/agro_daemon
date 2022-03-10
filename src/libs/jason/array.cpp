#include "jason.hpp"

namespace jason{

bool array_t::is(json_type const& value) noexcept
{
	return value.IsArray();
}

array_t::return_type array_t::get(json_type const& value) noexcept
{
	return array_ref_t{value.GetArray()};
}

void array_t::push_back(const char* data) noexcept
{
	value_.PushBack(rapidjson::Value(data, *alloc_).Move(), *alloc_);
}

void array_t::push_back(const char* data, std::size_t size) noexcept
{
	value_.PushBack(rapidjson::Value(data, size, *alloc_).Move(), *alloc_);
}

void array_t::push_back(std::string const& str) noexcept
{
	push_back(str.data(), str.size());
}

json_type const& array_ref_t::operator[](unsigned index) const noexcept
{
	return value_[index];
}

json_type& array_t::operator[](unsigned index) noexcept
{
	return value_[index];
}

json_type const& array_t::operator[](unsigned index) const noexcept
{
	return value_[index];
}

}//jason
