#ifndef JASON_META_FUNCTIONS_HPP__
#define JASON_META_FUNCTIONS_HPP__

#include <type_traits>

namespace jason{

template <typename... T>
struct has_type;

template <typename T, typename... Us>
struct has_type<T, Us...> : std::disjunction<std::is_same<T, Us>...> {};

template<typename T, typename _ = void>
struct is_container : std::false_type {};

template<typename T>
struct is_container<
        T,
        std::void_t<
            typename T::value_type,
            typename T::size_type,
//            typename T::allocator_type,
//            typename T::iterator,
            typename T::const_iterator,
            decltype(std::declval<T>().size()),
//            decltype(std::declval<T>().begin()),
//            decltype(std::declval<T>().end()),
            decltype(std::declval<T>().cbegin()),
            decltype(std::declval<T>().cend())>>
            : std::true_type {};

template<typename T, typename _ = void>
struct has_native : std::false_type {};

template<typename T>
struct has_native<
        T,
        std::void_t<
            decltype(std::declval<T>().native())>>
            : std::true_type {};

//struct fixed_string{
//    template<std::size_t N>
//    constexpr fixed_string(const char (&str)[N])
//        : data{str}, size{N}{}
//
//    const char* data;
//    std::size_t size;
//
//    constexpr operator const char* (void) const {
//        return data;
//    }
//};
//
//template <char ... C>
//struct string_literal {
//	static constexpr const char value[sizeof...(C) + 1] = {C..., '\0'};
//	constexpr operator const char* (void) const {
//		return value;
//	}
//};
//template <char ... C> constexpr const char string_literal<C...>::value[sizeof...(C) + 1];
//
//template <typename CharT, CharT... Cs>
//constexpr string_literal<Cs...> operator ""_key() {
//    return {};
//}

}//jason

#endif /* JASON_META_FUNCTIONS_HPP__ */
