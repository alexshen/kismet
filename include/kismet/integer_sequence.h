#ifndef KISMET_UTILITY_INTEGER_SEQUENCE_H
#define KISMET_UTILITY_INTEGER_SEQUENCE_H

#include <cstddef>
#include "kismet/core/assert.h"

namespace kismet
{

template<typename T, T... I>
struct integer_sequence
{ };

// To workaround VC120 bug of variadic templates
template<typename T, T N>
std::size_t get(std::size_t index, integer_sequence<T, N>)
{
    KISMET_ASSERT(index == 0);
    return N;
}

template<typename T, T N, T... I>
std::size_t get(std::size_t index, integer_sequence<T, N, I...>)
{
    KISMET_ASSERT(index < sizeof...(I)+1);
    return index == 0 ? N : get(index - 1, integer_sequence<T, I...>());
}

} // namespace kismet

#endif // KISMET_UTILITY_INTEGER_SEQUENCE_H
