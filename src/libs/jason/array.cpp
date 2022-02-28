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

void set(document& doc, key_type key, array_t& data) noexcept
{
	doc.AddMember(rapidjson::StringRef(key), data.native(), doc.GetAllocator());
}

void push_back(array_t& value, const char* data) noexcept
{
	value.native().PushBack(rapidjson::Value(data, value.allocator()).Move(), value.allocator());
}

void push_back(array_t& value, const char* data, std::size_t size) noexcept
{
	value.native().PushBack(rapidjson::Value(data, size, value.allocator()).Move(), value.allocator());
}

void push_back(array_t& value, std::string const& str) noexcept
{
	push_back(value, str.data(), str.size());
}

rapidjson::Value const& array_ref_t::operator[](unsigned index) const noexcept
{
	return value[index];
}

rapidjson::Value& array_t::operator[](unsigned index) noexcept
{
	return value[index];
}

rapidjson::Value const& array_t::operator[](unsigned index) const noexcept
{
	return value[index];
}

}//jason
