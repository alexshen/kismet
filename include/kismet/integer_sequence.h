#ifndef KISMET_INTEGER_SEQUENCE_H
#define KISMET_INTEGER_SEQUENCE_H

#include <cstddef>

namespace kismet
{

template<typename T, T... I>
struct integer_sequence
{
    enum { size = sizeof...(I) };
};

namespace detail
{

template<typename T, int I, T... S>
struct make_sequence_helper
{
    using type = typename make_sequence_helper<T, I-1, I-1, S...>::type;
};

template<typename T>
struct make_sequence_helper<T, 0>
{
    using type = integer_sequence<T>;
};

template<typename T, T... S>
struct make_sequence_helper<T, 1, S...>
{
    using type = integer_sequence<T, 0, S...>;
};

} // namespace detail

template<typename T, std::size_t N>
using make_integer_sequence = typename detail::make_sequence_helper<T, N>::type;

} // namespace kismet

#endif // KISMET_INTEGER_SEQUENCE_H 

