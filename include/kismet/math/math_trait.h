#ifndef KISMET_MATH_TRAIT_H
#define KISMET_MATH_TRAIT_H

#include "kismet/core/assert.h"

#include <cmath>
#include <cstdint>
#include <algorithm>
#include <type_traits>

namespace kismet
{

namespace math
{

/// A trait class describes some properties used in operations in this lib.
template<typename T>
class math_trait;

template<>
class math_trait<float>
{
public:
    static float zero_tolerance()
    {
        return s_zero_tol;
    }

    static void zero_tolerance(float t)
    {
        KISMET_ASSERT(t >= 0);
        s_zero_tol = t;
    }
private:
    static float s_zero_tol;
};

template<>
class math_trait<double>
{
public:
    static double zero_tolerance()
    {
        return s_zero_tol;
    }

    static void zero_tolerance(double t)
    {
        KISMET_ASSERT(t >= 0);
        s_zero_tol = t;
    }
private:
    static double s_zero_tol;
};

inline float invert(float f)
{
    KISMET_ASSERT(f);
    return 1.0f / f;
}

inline double invert(double d)
{
    KISMET_ASSERT(d);
    return 1.0 / d;
}

template<typename T>
inline bool is_zero(T v, T tol = math_trait<T>::zero_tolerance())
{
    using std::abs; // for ADL
    return abs(v) < tol;
}

/// Return true if lhs is approximately equal to rhs
// TODO: more accuracy
template<typename T>
inline bool approx(T lhs, T rhs, T tol = math_trait<T>::zero_tolerance())
{
    using std::abs;
    return abs(lhs - rhs) <= tol;
}

template<typename T>
inline T clamp(T v, T from, T to)
{
    using std::min;
    using std::max;

    KISMET_ASSERT(from <= to);
    return max(from, min(v, to));
}

template<typename T>
struct approximate
{
    bool operator ()(T lhs, T rhs) const
    {
        return approx(lhs, rhs);
    }
};

template<typename T>
inline T sign(T f)
{
    return f < T(0) ? T(-1) : (f == T(0) ? T(0) : T(1));
}

#define PI_CONSTANT(name, value) \
    constexpr double KISMET_##name = value; \
    constexpr float  KISMET_##name##_F = (float)value;

PI_CONSTANT(PI,          3.1415926535897932385);
PI_CONSTANT(PI_OVER_180, 0.0174532925199432957);
PI_CONSTANT(180_OVER_PI, 57.295779513082320876);
PI_CONSTANT(PI_OVER_2,   1.5707963267948966192);

#undef PI_CONSTANT

namespace detail
{
// promote type T to a floating point type
// if T is already a float point type, resulting type is unchanged.
template<typename T>
struct promote_to_float
{
    using type = float;
};

template<>
struct promote_to_float<double>
{
    using type = double;
};

template<>
struct promote_to_float<long double>
{
    using type = long double;
};

template<typename T>
using promote_to_float_t = typename promote_to_float<T>::type;

} // namespace detail

template<typename T>
inline detail::promote_to_float_t<T> deg2rad(T deg)
{
    using result_type = detail::promote_to_float_t<T>;
    return result_type(deg) * result_type(KISMET_PI_OVER_180);
}

template<typename T>
inline detail::promote_to_float_t<T> rad2deg(T rad)
{
    using result_type = detail::promote_to_float_t<T>;
    return result_type(rad) * result_type(KISMET_180_OVER_PI);
}

template<typename T>
inline detail::promote_to_float_t<T> radians(T degs)
{
    using result_type = detail::promote_to_float_t<T>;
    return result_type(degs) * result_type(KISMET_PI) / 180;
}

template<typename T>
inline detail::promote_to_float_t<T> degrees(T rads)
{
    using result_type = detail::promote_to_float_t<T>;
    return result_type(rads) * 180 / result_type(KISMET_PI);
}

// define math traits for integer type.
// do not use tolerance in is_zero(T, T) which is meaningless for integer type.
#define INT_MATH_TRAIT(T)          \
    template<> class math_trait<T> \
    {                              \
    public:                        \
        static T zero_tolerance()  \
        {                          \
            return (T)0;           \
        }                          \
    };                             \
                                   \
    inline bool is_zero(T v, T = (T)0)    \
    {                              \
        return v == (T)0;          \
    }

INT_MATH_TRAIT(short)
INT_MATH_TRAIT(unsigned short)
INT_MATH_TRAIT(int)
INT_MATH_TRAIT(unsigned int)
INT_MATH_TRAIT(long)
INT_MATH_TRAIT(unsigned long)
INT_MATH_TRAIT(long long)
INT_MATH_TRAIT(unsigned long long)

#undef INT_MATH_TRAIT

// floating point integer conversion
template<typename T>
union float_int;

// two-way specializations, so that we can instantiate
// using either floating point type or integer type to
// do conversion.
#define FLOAT_INT_SPECIALIZATION(F, I) \
    template<>                         \
    union float_int<F>                 \
    {                                  \
        using float_type = F;          \
        using int_type   = I;          \
                                       \
        float_type f;                  \
        int_type   i;                  \
    };                                 \
                                       \
    template<>                         \
    union float_int<I>                 \
    {                                  \
        using float_type = F;          \
        using int_type   = I;          \
                                       \
        float_type f;                  \
        int_type   i;                  \
    };

FLOAT_INT_SPECIALIZATION(float, std::int32_t)
FLOAT_INT_SPECIALIZATION(double, std::int64_t)

#undef FLOAT_INT_SPECIALIZATION

// Return the integer representation of the floating point number
template<typename T>
inline typename float_int<T>::int_type float_to_int_bits(T f)
{
    float_int<T> fi;
    fi.f = f;
    return fi.i;
}

// Return the float point number from the integer representation
template<typename T>
inline typename float_int<T>::float_type float_from_int_bits(T i)
{
    float_int<T> fi;
    fi.i = i;
    return fi.f;
}

} // namespace math

} // namespace kismet

#endif // KISMET_MATH_TRAIT_H
