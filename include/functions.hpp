#ifndef FUNCTIONS_HPP
#define FUNCTIONS_HPP

#include <tuple>
#include <utility>
#include <type_traits>
#include <cstddef>
#include <sstream>
#include <iomanip>

// from https://cpppatterns.com/patterns/apply-tuple-to-function.html
// Website also includes an explantation
// C++17 uses this in the form of std::apply, but using C++14 in this project.
template<typename F, typename Tuple, size_t ...S >
decltype(auto) apply_tuple_impl(F&& fn, Tuple&& t, std::index_sequence<S...>) {
    return std::forward<F>(fn)(std::get<S>(std::forward<Tuple>(t))...);
}

template<typename F, typename Tuple>
decltype(auto) apply_from_tuple(F&& fn, Tuple&& t) {
    constexpr std::size_t tSize = std::tuple_size<
                typename std::remove_reference<Tuple>::type
            >::value;
    return apply_tuple_impl(std::forward<F>(fn), std::forward<Tuple>(t),
                            std::make_index_sequence<tSize>());
}

// Convert a number to an hexadecimal integer in form of string
template<typename T>
std::string toHex(T&& num) {
    static_assert (std::is_integral<typename std::decay<T>::type>::value, "must be an integral type");
    std::stringstream stream;
    stream << "0x" << std::hex << static_cast<int>(std::forward<T>(num));
    return stream.str();
}


#endif // FUNCTIONS_HPP
