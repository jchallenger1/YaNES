#ifndef FUNCTIONS_HPP
#define FUNCTIONS_HPP

// A whole bunch of general functions used throughout this project

#include <tuple>
#include <utility>
#include <type_traits>
#include <cstddef>
#include <sstream>
#include <iomanip>



#define UNUSED(x) (void)(x)

// General kilobyte memory sizes
namespace memsize {
    constexpr uint16_t KB1 = 1048;
    constexpr uint16_t KB2 = 2048;
    constexpr uint16_t KB4 = 4096;
    constexpr uint16_t KB8 = 8192;
    constexpr uint16_t KB16 = 16384;
    constexpr uint16_t KB32 = 32768;
}

// from https://cpppatterns.com/patterns/apply-tuple-to-function.html
// Website also includes an explantation
// C++17 uses this in the form of std::apply, but using C++14 in this project.
template<typename F, typename Tuple, size_t ...S >
decltype(auto) apply_tuple_impl(F&& fn, Tuple&& t, std::index_sequence<S...>) {
    return std::forward<F>(fn)(std::get<S>(std::forward<Tuple>(t))...);
}

// Invokes a function passing a tuple as its arguments
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

constexpr inline bool inRange(const uint16_t& min, const uint16_t& max, const uint16_t& val) {
    return val <= max && val >= min;
}


#endif // FUNCTIONS_HPP
