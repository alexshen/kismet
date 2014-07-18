#ifndef KISMET_IS_COMPARABLE_H
#define KISMET_IS_COMPARABLE_H

#include <type_traits>
#include "kismet/utility.h"

namespace kismet
{

namespace detail
{

template<typename T, typename U>
struct is_comparable_impl
{
    template<typename T1, typename U1>
    static typename std::enable_if<
        std::is_convertible<
            decltype(std::declval<T1>() == std::declval<U1>())
          , bool>::value
      >::type check(T1 const&, U1 const&);

    static std::false_type check(...);

    enum { value = std::is_same<void, decltype(check(std::declval<T>(), std::declval<U>()))>::value };
};

}

template<typename... T>
struct is_comparable; 

template<typename T, typename U>
struct is_comparable<T, U>
    : boolean_constant_t<detail::is_comparable_impl<T, U>::value>
{
};

// Compare with self
template<typename T>
struct is_comparable<T> : is_comparable<T, T> {};

} // namespace kismet

#endif // KISMET_IS_COMPARABLE_H 
