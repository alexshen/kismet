#ifndef KISMET_MATH_UTILITY_H
#define KISMET_MATH_UTILITY_H

#include "kismet/math/math_trait.h"
#include "kismet/core/assert.h"

namespace kismet
{

namespace math
{

template<typename T>
inline void make_identity(T (&a)[2][2])
{
    a[0][0] = T(1);
    a[0][1] = T(0);
    a[1][0] = T(0);
    a[1][1] = T(1);
}

template<typename T>
inline void make_identity(T (&a)[3][3])
{
    a[0][0] = T(1);
    a[0][1] = T(0);
    a[0][2] = T(0);

    a[1][0] = T(0);
    a[1][1] = T(1);
    a[1][2] = T(0);

    a[2][0] = T(0);
    a[2][1] = T(0);
    a[2][2] = T(1);
}


template<typename T>
inline T lerp(T a, T b, T t)
{
    return a + (b - a) * t;
}

// calculate the t satisfies c = lerp(a, b, t)
template<typename T>
inline T inverse_lerp(T a, T b, T c)
{
    KISMET_ASSERT(a < b);
    return (c - a) / (b - a);
}

template<typename T>
int max_index(T a, T b, T c)
{
    if (a > b)
    {
        if (a >= c) { return 0; }
        else { return 2; }
    }
    else // a <=b 
    {
        if (b > c) { return 1; }
        else { return 2; }
    }
}

} // namespace math

} // namespace kismet

#endif // KISMET_MATH_UTILITY_H
