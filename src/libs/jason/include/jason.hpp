#ifndef AGRO_DAEMON_LIB_JASON_HPP__
#define AGRO_DAEMON_LIB_JASON_HPP__

#include "rapidjson/document.h"
//types
#include <cstdint>

#include <optional>
#include <functional>
#include <string>

namespace Jason{

using json_type = rapidjson::Value;
using key_type = const char*;
using document = rapidjson::Document;

//JSON types
template<typename T, typename R = T>
struct base_t{
	using type = T;
	using return_type = R;

	static bool is(json_type const&) noexcept;
	static return_type get(json_type const& value) noexcept;
};

using string_t = base_t<const char*>;
using integer_t = base_t<int>;
using integer64_t = base_t<std::int64_t>;
using unsigned_t = base_t<unsigned>;
using unsigned64_t = base_t<std::uint64_t>;
using float_t = base_t<float>;
using double_t = base_t<double>;
using boolean_t = base_t<bool>;
using object_t = base_t<rapidjson::Value, rapidjson::Value::ConstObject>;

struct array_t
{
	using type = rapidjson::Value;
	using return_type = rapidjson::Value::ConstArray;
	using alloc_type = rapidjson::Value::AllocatorType;

	array_t(alloc_type& alloc_) : alloc{alloc_}
	{
		value.SetArray();
	}

	array_t(document& doc) : array_t{doc.GetAllocator()}{}

	static bool is(json_type const&) noexcept;
	static return_type get(json_type const& value) noexcept;

	auto get() noexcept
	{
		return value.GetArray();
	}

	rapidjson::Value value;
	alloc_type& alloc;
};

struct null_t{
	static bool is(json_type const&) noexcept;
};

struct number_t{
	static bool is(json_type const&) noexcept;
};

template<typename T>
struct field{
	using type = T;
	key_type key;

	template<typename Str>
	constexpr field(Str k) : key{k}{}
};

bool parse(document&, const char*) noexcept;
bool parse(document&, const char*, std::size_t) noexcept;
bool parse(document&, std::string const&) noexcept;

template<bool Pretty = false>
std::string stringify(rapidjson::Document const& doc) noexcept;

namespace literals{

constexpr field<string_t> operator "" _s(const char*, std::size_t) noexcept;
constexpr field<integer_t> operator "" _i(const char*, std::size_t) noexcept;
constexpr field<integer64_t> operator "" _i64(const char*, std::size_t) noexcept;
constexpr field<unsigned_t> operator "" _u(const char*, std::size_t) noexcept;
constexpr field<unsigned64_t> operator "" _u64(const char*, std::size_t) noexcept;
constexpr field<float_t> operator "" _f(const char*, std::size_t) noexcept;
constexpr field<double_t> operator "" _d(const char*, std::size_t) noexcept;
constexpr field<number_t> operator "" _n(const char*, std::size_t) noexcept;
constexpr field<boolean_t> operator "" _b(const char*, std::size_t) noexcept;
constexpr field<null_t> operator "" _null(const char*, std::size_t) noexcept;
constexpr field<object_t> operator "" _o(const char*, std::size_t) noexcept;
constexpr field<array_t> operator "" _a(const char*, std::size_t) noexcept;

}//literals

/**
 * Verify
 */

bool has(object_t::type const&, key_type) noexcept;

template<typename T>
bool is(json_type const&) noexcept;
bool is_string(json_type const&) noexcept;
bool is_int(json_type const&) noexcept;
bool is_int64(json_type const&) noexcept;
bool is_uint(json_type const&) noexcept;
bool is_uint64(json_type const&) noexcept;
bool is_float(json_type const&) noexcept;
bool is_double(json_type const&) noexcept;
bool is_number(json_type const&) noexcept;
bool is_bool(json_type const&) noexcept;
bool is_null(json_type const&) noexcept;
bool is_object(json_type const&) noexcept;
bool is_array(json_type const&) noexcept;

template<typename T>
bool verify(object_t::type const&, field<T> const&) noexcept;
template<typename Arg, typename... Args>
bool verify(object_t::type const&, field<Arg> const&, Args&& ...) noexcept;

template<typename T>
bool operator&&(object_t::type const&, field<T> const&) noexcept;

/**
 * get
 */
template<typename T, bool Verify = true>
std::optional<typename T::return_type> get(json_type const&) noexcept;
template<typename T, bool Verify = true>
std::optional<typename T::return_type> get(object_t::type const&, field<T> const&) noexcept;

template<bool Verify = true>
std::optional<string_t::return_type> get_string(json_type const&) noexcept;
template<bool Verify = true>
std::optional<integer_t::return_type> get_int(json_type const&) noexcept;
template<bool Verify = true>
std::optional<integer64_t::return_type> get_int64(json_type const&) noexcept;
template<bool Verify = true>
std::optional<unsigned_t::return_type> get_uint(json_type const&) noexcept;
template<bool Verify = true>
std::optional<unsigned64_t::return_type> get_uint64(json_type const&) noexcept;
template<bool Verify = true>
std::optional<float_t::return_type> get_float(json_type const&) noexcept;
template<bool Verify = true>
std::optional<double_t::return_type> get_double(json_type const&) noexcept;
template<bool Verify = true>
std::optional<boolean_t::return_type> get_bool(json_type const&) noexcept;
template<bool Verify = true>
std::optional<object_t::return_type> get_object(json_type const&) noexcept;
template<bool Verify = true>
std::optional<array_t::return_type> get_array(json_type const&) noexcept;

/**
 * Set
 */
void set(document&, key_type, const char*) noexcept;
void set(document&, key_type, const char*, std::size_t) noexcept;
void set(document&, key_type, std::string const&) noexcept;

template<typename T>
void set(document&, key_type, T) noexcept;
void set(document& doc, key_type key, object_t::type& data) noexcept;
void set(document& doc, key_type key, array_t& data) noexcept;

template<typename Allocator>
void set(object_t::type&, key_type, const char*, Allocator&) noexcept;
template<typename Allocator>
void set(object_t::type&, key_type, const char*, std::size_t, Allocator&) noexcept;
template<typename Allocator>
void set(object_t::type&, key_type, std::string const&, Allocator&) noexcept;

template<typename T, typename Allocator>
void set(object_t::type&, key_type, T, Allocator&) noexcept;
template<typename T, typename Allocator>
void set(object_t::type&, key_type, object_t::type const&, Allocator&) noexcept;

void push_back(array_t&, const char*) noexcept;
void push_back(array_t&, const char*, std::size_t) noexcept;
void push_back(array_t&, std::string const&) noexcept;

template<typename T>
void push_back(array_t&, T) noexcept;

}//Jason

#include "literals.hpp"
#include "impl/jason_impl.hpp"

#endif /* AGRO_DAEMON_LIB_JASON_HPP__ */
