#include "jason.hpp"

namespace jason{

bool has(object_t::type const& obj, key_type name) noexcept
{
	return obj.HasMember(name);
}

bool is_int(json_type const& value) noexcept
{
	return value.IsInt();
}

bool is_int64(json_type const& value) noexcept
{
	return value.IsInt64();
}

bool is_uint(json_type const& value) noexcept
{
	return value.IsUint();
}

bool is_uint64(json_type const& value) noexcept
{
	return value.IsUint64();
}

bool is_float(json_type const& value) noexcept
{
	return value.IsFloat();
}

bool is_double(json_type const& value) noexcept
{
	return value.IsDouble();
}

bool is_number(json_type const& value) noexcept
{
	return value.IsNumber();
}

bool is_bool(json_type const& value) noexcept
{
	return value.IsBool();
}

bool is_null(json_type const& value) noexcept
{
	return value.IsNull();
}

bool is_object(json_type const& value) noexcept
{
	return value.IsObject();
}

bool is_array(json_type const& value) noexcept
{
	return value.IsArray();
}

/**
 * Check
 */
bool null_t::is(json_type const& value) noexcept
{
	return value.IsNull();
}

bool number_t::is(json_type const& value) noexcept
{
	return value.IsNumber();
}

/**
 * string_t
 */
template<>
bool string_t::is(json_type const& value) noexcept
{
	return value.IsString();
}

bool is_string(json_type const& value) noexcept
{
	return value.IsString();
}

template<>
string_t::return_type string_t::get(json_type const& value) noexcept
{
	return value.GetString();
}

void set(document& doc, key_type key, const char* str) noexcept
{
	set(doc, key, str, doc.GetAllocator());
}

void set(document& doc, key_type key, const char* str, std::size_t size) noexcept
{
	set(doc, key, str, size, doc.GetAllocator());
}

void set(document& doc, key_type key, std::string const& str) noexcept
{
	set(doc, key, str.data(), str.size());
}

/**
 * integer_t
 */
template<>
bool integer_t::is(json_type const& value) noexcept
{
	return value.IsInt();
}

template<>
integer_t::return_type integer_t::get(json_type const& value) noexcept
{
	return value.GetInt();
}



template<>
bool integer64_t::is(json_type const& value) noexcept
{
	return value.IsInt64();
}

template<>
integer64_t::return_type integer64_t::get(json_type const& value) noexcept
{
	return value.GetInt64();
}

template<>
bool unsigned_t::is(json_type const& value) noexcept
{
	return value.IsUint();
}

template<>
unsigned_t::return_type unsigned_t::get(json_type const& value) noexcept
{
	return value.GetUint();
}

template<>
bool unsigned64_t::is(json_type const& value) noexcept
{
	return value.IsUint64();
}

template<>
unsigned64_t::return_type unsigned64_t::get(json_type const& value) noexcept
{
	return value.GetUint64();
}

template<>
bool float_t::is(json_type const& value) noexcept
{
	return value.IsFloat();
}

template<>
float_t::return_type float_t::get(json_type const& value) noexcept
{
	return value.GetFloat();
}

template<>
bool double_t::is(json_type const& value) noexcept
{
	return value.IsDouble();
}

template<>
double_t::return_type double_t::get(json_type const& value) noexcept
{
	return value.GetDouble();
}

template<>
bool boolean_t::is(json_type const& value) noexcept
{
	return value.IsBool();
}

template<>
boolean_t::return_type boolean_t::get(json_type const& value) noexcept
{
	return value.GetBool();
}

template<>
bool object_t::is(json_type const& value) noexcept
{
	return value.IsObject();
}

template<>
object_t::return_type object_t::get(json_type const& value) noexcept
{
	return object_t::return_type{value.GetObject()};
}

void set(document& doc, key_type key, object_t::type& data) noexcept
{
	doc.AddMember(rapidjson::StringRef(key), data, doc.GetAllocator());
}

}//jason
