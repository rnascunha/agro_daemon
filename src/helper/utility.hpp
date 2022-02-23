#ifndef AGRO_DAEMON_HELPER_UTILITY_HPP__
#define AGRO_DAEMON_HELPER_UTILITY_HPP__

#include <cstring>

template<typename Type>
struct config{
	Type		type;
	const char* name;
};

inline constexpr bool is_equal(const char* first, const char* second) noexcept
{
	while(*first++ == *second++ && (*first != '\0' && *second != '\0'));
	return first == second;
}

template<typename Config, std::size_t Size>
constexpr const Config* get_config(const char* name,
                    const Config(&config)[Size]) noexcept
{
    for(std::size_t i = 0; i < Size; i++)
    {
        if(std::strcmp(name, config[i].name) == 0)
            return &config[i];
    }
    return nullptr;
}

template<typename Type, typename Config, std::size_t Size>
constexpr const Config* get_config(Type type,
                    const Config(&config)[Size]) noexcept
{
    for(std::size_t i = 0; i < Size; i++)
    {
        if(type == config[i].type)
            return &config[i];
    }
    return nullptr;
}

#endif /* AGRO_DAEMON_HELPER_UTILITY_HPP__ */
