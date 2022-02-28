#ifndef AGRO_DAEMON_LIB_JASON_IMPL_HPP__
#define AGRO_DAEMON_LIB_JASON_IMPL_HPP__

#include "../jason.hpp"
#include "rapidjson/writer.h"
#include "rapidjson/prettywriter.h"
#include "meta.hpp"

namespace jason{

template<typename T>
constexpr void assert_type() noexcept
{
	static_assert(has_type<T, string_t, integer_t,
				integer64_t, unsigned_t, unsigned64_t,
				float_t, double_t, number_t,
				boolean_t, null_t, object_t, array_t>::value, "JSON type not defined");
}

template<typename T>
constexpr void assert_get_type() noexcept
{
	static_assert(has_type<T, string_t, integer_t,
				integer64_t, unsigned_t, unsigned64_t,
				float_t, double_t,
				boolean_t, object_t, array_t>::value, "JSON get type not defined");
}

template<typename T>
constexpr void assert_set_type() noexcept
{
	static_assert(has_type<T, integer_t,
				integer64_t, unsigned_t, unsigned64_t,
				float_t, double_t,
				boolean_t, object_t, array_t>::value, "JSON set type not defined");
}

template<typename T>
constexpr void assert_base_type() noexcept
{
	static_assert(has_type<T, integer_t::type,
				integer64_t::type, unsigned_t::type, unsigned64_t::type,
				float_t::type, double_t::type,
				boolean_t::type, object_t::type, array_t::type>::value, "JSON base type not defined");
}

template<bool Pretty /* = false */>
std::string stringify(rapidjson::Document const& doc) noexcept
{
	rapidjson::StringBuffer sb;
	if constexpr(Pretty)
	{
		rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(sb);
		doc.Accept(writer);
	}
	else
	{
		rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
		doc.Accept(writer);
	}

	return sb.GetString();
}

template<typename T>
bool is(json_type const& value) noexcept
{
	assert_type<T>();

	return T::is(value);
}

template<typename T>
bool verify(object_t::type const& value, field<T> const& field) noexcept
{
	assert_type<T>();

	if(!is_object(value)) return false;
	if(!has(value, field.key)) return false;
	return is<T>(value[field.key]);
}

template<typename Arg, typename... Args>
bool verify(object_t::type const& value, field<Arg> const& arg, Args&& ...args) noexcept
{
	if(!verify(value, arg)) return false;
	return verify(value, std::forward<Args>(args)...);
}

template<typename T>
bool operator&&(object_t::type const& value, field<T> const& field) noexcept
{
	return verify(value, field);
}

template<typename T, bool Verify = true>
std::optional<typename T::return_type>
get(json_type const& value) noexcept
{
	assert_get_type<T>();

	if constexpr(Verify)
	{
		return is<T>(value) ?
				std::optional<typename T::return_type>{T::get(value)} :
				std::nullopt;
	}
	else
	{
		return std::optional<typename T::return_type>{T::get(value)};
	}
}

template<typename T, bool Verify /* = true */>
std::optional<typename T::return_type>
get(object_t::type const& value, field<T> const& field) noexcept
{
	assert_get_type<T>();

	if constexpr(Verify)
	{
		return verify(value, field) ?
				get<T, Verify>(value[field.key]) :
				std::nullopt;
	}
	else
	{
		return get<T, Verify>(value[field.key]);
	}
}

template<bool Verify /* = true */>
std::optional<string_t::return_type> get_string(json_type const& value) noexcept
{
	return get<string_t, Verify>(value);
}

template<bool Verify /* = true */>
std::optional<integer_t::return_type> get_int(json_type const& value) noexcept
{
	return get<integer_t, Verify>(value);
}

template<bool Verify /* = true */>
std::optional<integer64_t::return_type> get_int64(json_type const& value) noexcept
{
	return get<integer_t, Verify>(value);
}

template<bool Verify /* = true */>
std::optional<unsigned_t> get_uint(json_type const& value) noexcept
{
	return get<unsigned_t, Verify>(value);
}

template<bool Verify /* = true */>
std::optional<unsigned64_t::return_type> get_uint64(json_type const& value) noexcept
{
	return get<unsigned64_t, Verify>(value);
}

template<bool Verify /* = true */>
std::optional<float_t::return_type> get_float(json_type const& value) noexcept
{
	return get<float_t, Verify>(value);
}

template<bool Verify /* = true */>
std::optional<double_t::return_type> get_double(json_type const& value) noexcept
{
	return get<double_t, Verify>(value);
}

template<bool Verify /* = true */>
std::optional<boolean_t::return_type> get_bool(json_type const& value) noexcept
{
	return get<boolean_t, Verify>(value);
}

template<bool Verify /* = true */>
std::optional<object_t::return_type> get_object(json_type const& value) noexcept
{
	return get<object_t, Verify>(value);
}

template<bool Verify /* = true */>
std::optional<array_t::return_type> get_array(json_type const& value) noexcept
{
	return get<array_t, Verify>(value);
}

/**
 * Set
 */
template<typename Allocator>
void set(object_t::type& value, key_type key, const char* str, Allocator& alloc) noexcept
{
	value.AddMember(rapidjson::StringRef(key),
				rapidjson::StringRef(str),
				alloc);
}

template<typename Allocator>
void set(object_t::type& value, key_type key, const char* str, std::size_t size, Allocator& alloc) noexcept
{
	value.AddMember(rapidjson::StringRef(key),
					rapidjson::StringRef(str, size),
					alloc);
}

template<typename Allocator>
void set(object_t::type& value, key_type key, std::string const& str, Allocator& alloc) noexcept
{
	set(value, key, str.data(), str.size(), alloc);
}

template<typename T, typename Allocator>
void set(object_t::type& value, key_type key, T data, Allocator& alloc) noexcept
{
	value.AddMember(rapidjson::StringRef(key), data, alloc);
}

template<typename T, typename Allocator>
void set(object_t::type& value, key_type key, object_t::type& data, Allocator& alloc) noexcept
{
	value.AddMember(rapidjson::StringRef(key), data, alloc);
}

template<typename T>
void set(document& doc, key_type key, T data) noexcept
{
	set(doc, key, data, doc.GetAllocator());
}

}//jason

#endif /* AGRO_DAEMON_LIB_JASON_IMPL_HPP__ */
