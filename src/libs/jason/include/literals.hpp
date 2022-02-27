#ifndef AGRO_DAEMON_LIB_JASON_LITERALS_HPP__
#define AGRO_DAEMON_LIB_JASON_LITERALS_HPP__

#include "jason.hpp"

namespace Jason{
namespace literals{

constexpr field<string_t> operator "" _s(const char* key, std::size_t) noexcept
{
	return field<string_t>{key};
}

constexpr field<integer_t> operator "" _i(const char* key, std::size_t) noexcept
{
	return field<integer_t>{key};
}

constexpr field<integer64_t> operator "" _i64(const char* key, std::size_t) noexcept
{
	return field<integer64_t>{key};
}

constexpr field<unsigned_t> operator "" _u(const char* key, std::size_t) noexcept
{
	return field<unsigned_t>{key};
}

constexpr field<unsigned64_t> operator "" _u64(const char* key, std::size_t) noexcept
{
	return field<unsigned64_t>{key};
}

constexpr field<float_t> operator "" _f(const char* key, std::size_t) noexcept
{
	return field<float_t>{key};
}

constexpr field<double_t> operator "" _d(const char* key, std::size_t) noexcept
{
	return field<double_t>{key};
}

constexpr field<number_t> operator "" _n(const char* key, std::size_t) noexcept
{
	return field<number_t>{key};
}

constexpr field<boolean_t> operator "" _b(const char* key, std::size_t) noexcept
{
	return field<boolean_t>{key};
}

constexpr field<null_t> operator "" _null(const char* key, std::size_t) noexcept
{
	return field<null_t>{key};
}

constexpr field<object_t> operator "" _o(const char* key, std::size_t) noexcept
{
	return field<object_t>{key};
}

constexpr field<array_t> operator "" _a(const char* key, std::size_t) noexcept
{
	return field<array_t>{key};
}

}//literals
}//Jason

#endif /* AGRO_DAEMON_LIB_JASON_LITERALS_HPP__ */
