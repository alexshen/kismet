#ifndef KISMET_MATH_VECTOR_COMMON_H
#define KISMET_MATH_VECTOR_COMMON_H

#include <cstddef>
#include <cmath>
#include <utility>
#include "kismet/math/math_trait.h"

namespace kismet
{
namespace math
{
namespace detail
{

// Requirements
//   1. vector must support member function operator []
//   2. vector must support member function size()
//   3. vector must has value_type member type

// Return the dot product of given two vectors
template<typename T>
inline typename T::value_type dot(T const& v1, T const& v2)
{
    KISMET_ASSERT(v1.size() == v2.size());
    
    typename T::value_type res(0);
    for (std::size_t i = 0; i < v1.size(); ++i)
    {
        res += v1[i] * v2[i];
    }
    return res;
}

// Return the squared magnitude of the given vector
template<typename T>
inline typename T::value_type squared_mag(T const& v)
{
    typename T::value_type res(0);
    for (std::size_t i = 0; i < v.size(); ++i)
    {
        res += v[i] * v[i];
    }
    return res;
}

// Return the magnitude of the given vector
template<typename T>
inline typename T::value_type mag(T const& v)
{
    return std::sqrtf(squared_mag(v));
}

// normalize the given vector, assume that the vector's magnitude is not zero
template<typename T>
inline void normalize(T& v, typename T::value_type tolerance)
{
    KISMET_ASSERT(!is_zero(mag(v), tolerance));

    auto inv_mag(invert(mag(v)));
    for (std::size_t i = 0; i < v.size(); ++i)
    {
        v[i] *= inv_mag;
    }
}

// normalize the given vector
// Return true if normalization succeeds, otherwise
// return false in which case v is left unchanged.
template<typename T>
inline bool safe_normalize(T& v, typename T::value_type tolerance = math_trait<T>::zero_tolerance())
{
    auto sqr_mag(squared_mag(v));
    if (is_zero(sqr_mag, tolerance))
    {
        return false;
    }

    auto inv_mag(invert(sqrtf(sqr_mag)));

    for (std::size_t i = 0; i <= v.size() - 1; ++i)
    {
        v[i] *= inv_mag;
    }
    return true;
}

} // namespace detail
} // namespace math
} // namespace kismet

#endif // KISMET_MATH_VECTOR_COMMON_H

