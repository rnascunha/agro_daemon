#ifndef AGRO_DAEMON_HELPER_ENUM_OP_HPP__
#define AGRO_DAEMON_HELPER_ENUM_OP_HPP__

#include <type_traits>

template<typename Enum>
constexpr Enum operator|(Enum lhs, Enum rhs) noexcept
{
	static_assert(std::is_enum<Enum>::value, "Must be 'enum' type");
	return static_cast<Enum>(static_cast<int>(lhs) | static_cast<int>(rhs));
}

template<typename Enum>
constexpr bool operator&(Enum lhs, Enum rhs) noexcept
{
	static_assert(std::is_enum<Enum>::value, "Must be 'enum' type");
	return static_cast<bool>(static_cast<int>(lhs) & static_cast<int>(rhs));
}

#endif /* AGRO_DAEMON_HELPER_ENUM_OP_HPP__ */
