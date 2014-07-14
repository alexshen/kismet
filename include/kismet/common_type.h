#ifndef KISMET_COMMON_TYPE_H
#define KISMET_COMMON_TYPE_H

#include <type_traits>
#include "kismet/config.h"

namespace kismet
{

// implements the common_type with SFINAE as proposed by N3843
// see http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2014/n3843.pdf 
// for more details
namespace detail
{

template<typename T, typename U>
using ct2 = decltype(std::declval<bool>() ? std::declval<T>() : std::declval<U>());

template<typename T>
struct always_void
{
    using type = void;
};

template<typename T, typename...>
struct ct {};

template<typename T>
struct ct<void, T>
{
    using type = typename std::decay<T>::type;
};

template<typename T, typename U, typename... V>
struct ct<typename always_void<ct2<T, U>>::type, T, U, V...>
    : ct<void, ct2<T, U>, V...>
{};

} // namespace detail

template<typename... T>
struct common_type : detail::ct<void, T...> {};

#ifndef KISMET_MSC

template<typename... T>
using common_type_t = typename common_type<T...>::type;

#else

template<typename T, typename U>
using common_type_t = typename common_type<T, U>::type;

#endif

} // namespace kismet

#endif // KISMET_COMMON_TYPE_H
