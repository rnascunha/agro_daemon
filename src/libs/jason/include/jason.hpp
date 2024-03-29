#ifndef AGRO_DAEMON_LIB_JASON_HPP__
#define AGRO_DAEMON_LIB_JASON_HPP__

#include "rapidjson/document.h"

#include <cstdint> //types

#include <optional>
#include <functional>
#include <string>

#include "meta.hpp"

namespace jason{

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

template<typename T>
struct field{
	using type = T;
	key_type key;

	constexpr field(key_type k) : key{k}{}
};

class object_ref_t{
	public:
		using native_type = rapidjson::Value::ConstObject;
		using iterator = rapidjson::Value::Object::MemberIterator;
		using const_iterator = rapidjson::Value::Object::MemberIterator;

		object_ref_t(native_type&& value_) : value_{value_}{}

		bool has(key_type) const noexcept;

		auto begin() const noexcept{ return value_.MemberBegin(); }
		auto end() const noexcept{ return value_.MemberEnd(); }

		json_type const& operator[](key_type index) const noexcept;
		template<typename T>
		std::optional<typename T::return_type> operator[](field<T> const& idx) const noexcept;

		native_type const& native() const noexcept{ return value_; }
	private:
		native_type value_;
};

class array_t;

class object_t{
	public:
		using type = object_t;
		using native_type = rapidjson::Value;
		using return_type = object_ref_t;
		using alloc_type = rapidjson::Value::AllocatorType;
		using iterator = rapidjson::Value::Object::MemberIterator;
		using const_iterator = rapidjson::Value::Object::ConstMemberIterator;

		object_t(alloc_type& alloc_) : alloc_{&alloc_}{ value_.SetObject(); }
		object_t(document& doc) : object_t{doc.GetAllocator()}{}
		object_t(document&& doc){ swap(doc); }	//using swap because move doesn't work

		bool has(key_type) const noexcept;

		void set(key_type, const char*) noexcept;
		void set(key_type, const char*, std::size_t) noexcept;
		void set(key_type, std::string const&) noexcept;

		template<typename T>
		void set(key_type, T) noexcept;
		void set(key_type, object_t&) noexcept;
		void set(key_type, array_t&) noexcept;

		auto begin() const noexcept{ return value_.MemberBegin(); }
		auto end() const noexcept{ return value_.MemberEnd(); }
		auto begin() noexcept{ return value_.MemberBegin(); }
		auto end() noexcept{ return value_.MemberEnd(); }

		json_type& operator[](key_type key) noexcept;
		json_type const& operator[](key_type key) const noexcept;
		template<typename T>
		std::optional<typename T::return_type> operator[](field<T> const& idx) const noexcept;

		native_type& native() noexcept{ return value_; }
		native_type const& native() const noexcept{ return value_; }

		alloc_type& allocator() noexcept{ return *alloc_; }

		static bool is(json_type const&) noexcept;
		static return_type get(json_type const& value) noexcept;

		object_t(){}
		void swap(document& doc) noexcept;
	protected:
		native_type value_;
		alloc_type* alloc_ = nullptr;
};

class object_doc : public object_t{
	public:
		object_doc()
		{
			doc_.SetObject();
			value_.Swap(doc_);
			alloc_ = &doc_.GetAllocator();
		}

		document& doc() noexcept{ return doc_; }
	private:
		document doc_{};
};

using index_t = unsigned long long int;
template<typename T>
struct index{
	using type = T;

	index_t i;

	constexpr index(index_t k) : i{k}{}
};

class array_ref_t{
	public:
		using native_type = rapidjson::Value::ConstArray;
		using const_iterator = rapidjson::Value::Array::ConstValueIterator;

		array_ref_t(native_type&& value) : value_{std::move(value)}{}

		const_iterator begin() const noexcept { return value_.Begin(); }
		const_iterator end() const noexcept { return value_.End(); }

		const_iterator cbegin() const noexcept { return value_.Begin(); }
		const_iterator cend() const noexcept { return value_.End(); }

		std::size_t size() const noexcept{ return value_.Size(); }

		json_type const& operator[](unsigned index) const noexcept;
		template<typename T>
		std::optional<typename T::return_type> operator[](index<T> const& idx) const noexcept;

		native_type const& native() const noexcept{ return value_; }
	private:
		native_type value_;
};

class array_t
{
	public:
		using type = array_t;
		using native_type = rapidjson::Value;
		using return_type = array_ref_t;
		using alloc_type = rapidjson::Value::AllocatorType;
		using iterator = rapidjson::Value::Array::ValueIterator;
		using const_iterator = rapidjson::Value::Array::ConstValueIterator;

		array_t(alloc_type& alloc) : alloc_{&alloc}{ value_.SetArray(); }
		array_t(document& doc) : array_t{doc.GetAllocator()}{}
		array_t(document&& doc){ swap(doc); }

		iterator begin() noexcept { return value_.Begin(); }
		iterator end() noexcept { return value_.End(); }
		const_iterator begin() const noexcept { return value_.Begin(); }
		const_iterator end() const noexcept { return value_.End(); }

		const_iterator cbegin() const noexcept { return value_.Begin(); }
		const_iterator cend() const noexcept { return value_.End(); }

		std::size_t size() const noexcept{ return value_.Size(); }

		void push_back(const char*) noexcept;
		void push_back(const char*, std::size_t) noexcept;
		void push_back(std::string const&) noexcept;

		template<typename T>
		void push_back(T t) noexcept;
		template<typename T, typename... Ts>
		void push_back(T&& t, Ts&&... ts) noexcept;
		template<typename ...Ts>
		void push_back(Ts&&...) noexcept;

		template<typename InputIter>
		void push(InputIter, InputIter) noexcept;
		template<typename Container>
		void push(Container const&) noexcept;

		json_type& operator[](unsigned index) noexcept;
		json_type const& operator[](unsigned index) const noexcept;
		template<typename T>
		std::optional<typename T::return_type> operator[](index<T> const& idx) const noexcept;

		template<typename T>
		array_t& operator<<(T t) noexcept;

		native_type& native() noexcept{ return value_; }
		native_type const& native() const noexcept{ return value_; }

		alloc_type& allocator() noexcept{ return *alloc_; }

		static bool is(json_type const&) noexcept;
		static return_type get(json_type const& value_) noexcept;

		array_t(){}
		void swap(document& doc) noexcept;
	protected:

		alloc_type* alloc_ = nullptr;
		native_type value_;
};

class array_doc : public array_t{
	public:
		array_doc()
		{
			doc_.SetArray();
			value_.Swap(doc_);
			alloc_ = &doc_.GetAllocator();
		}

		document& doc() noexcept{ return doc_; }
	private:
		document doc_{};
};

struct null_t{
	static bool is(json_type const&) noexcept;
};

struct number_t{
	static bool is(json_type const&) noexcept;
};

bool parse(document&, const char*) noexcept;
bool parse(document&, const char*, std::size_t) noexcept;
bool parse(document&, std::string const&) noexcept;

template<bool Pretty = false>
std::string stringify(json_type const& doc) noexcept;
template<bool Pretty = false, typename T, std::enable_if_t<has_native<T>::value, int> = 0>
std::string stringify(T const& doc) noexcept
{
	return stringify<Pretty>(doc.native());
}

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

constexpr index<string_t> operator "" _s(index_t) noexcept;
constexpr index<integer_t> operator "" _i(index_t) noexcept;
constexpr index<integer64_t> operator "" _i64(index_t) noexcept;
constexpr index<unsigned_t> operator "" _u(index_t) noexcept;
constexpr index<unsigned64_t> operator "" _u64(index_t) noexcept;
constexpr index<float_t> operator "" _f(index_t) noexcept;
constexpr index<double_t> operator "" _d(index_t) noexcept;
constexpr index<boolean_t> operator "" _b(index_t) noexcept;
constexpr index<object_t> operator "" _o(index_t) noexcept;
constexpr index<array_t> operator "" _a(index_t) noexcept;

}//literals

/**
 * Verify
 */
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
bool verify(object_t const& value, field<T> const& field) noexcept;
template<typename Arg, typename... Args>
bool verify(object_t const& value, field<Arg> const& arg, Args&& ...args) noexcept;

template<typename T>
bool operator&&(object_t const&, field<T> const&) noexcept;

/**
 * get
 */
template<typename T, bool Verify = true>
std::optional<typename T::return_type> get(json_type const&) noexcept;
template<typename T, bool Verify = true>
std::optional<typename T::return_type> get(object_t const&, field<T> const&) noexcept;

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

}//jason

#include "literals.hpp"
#include "impl/jason_impl.hpp"
#include "impl/array_impl.hpp"
#include "impl/object_impl.hpp"

#endif /* AGRO_DAEMON_LIB_JASON_HPP__ */
