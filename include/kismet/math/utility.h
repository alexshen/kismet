#ifndef KISMET_MATH_UTILITY_H
#define KISMET_MATH_UTILITY_H

#include "kismet/math/math_trait.h"

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

} // namespace math

} // namespace kismet

#endif // KISMET_MATH_UTILITY_H
