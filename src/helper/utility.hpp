#ifndef AGRO_DAEMON_HELPER_UTILITY_HPP__
#define AGRO_DAEMON_HELPER_UTILITY_HPP__

inline constexpr bool is_equal(const char* first, const char* second) noexcept
{
	while(*first++ == *second++ && (*first != '\0' && *second != '\0'));
	return first == second;
}

#endif /* AGRO_DAEMON_HELPER_UTILITY_HPP__ */
