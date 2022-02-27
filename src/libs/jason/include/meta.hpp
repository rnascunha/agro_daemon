#ifndef JASON_META_FUNCTIONS_HPP__
#define JASON_META_FUNCTIONS_HPP__

#include <type_traits>

namespace Jason{

template <typename... T>
struct has_type;

template <typename T, typename... Us>
struct has_type<T, Us...> : std::disjunction<std::is_same<T, Us>...> {};

struct fixed_string{
    template<std::size_t N>
    constexpr fixed_string(const char (&str)[N])
        : data{str}, size{N}{}

    const char* data;
    std::size_t size;

    constexpr operator const char* (void) const {
        return data;
    }
};

template <char ... C>
struct string_literal {
	static constexpr const char value[sizeof...(C) + 1] = {C..., '\0'};
	constexpr operator const char* (void) const {
		return value;
	}
};
template <char ... C> constexpr const char string_literal<C...>::value[sizeof...(C) + 1];

template <typename CharT, CharT... Cs>
constexpr string_literal<Cs...> operator ""_key() {
    return {};
}

}//Jason

#endif /* JASON_META_FUNCTIONS_HPP__ */
