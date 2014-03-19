#ifndef KISMET_MATH_TRAIT_H
#define KISMET_MATH_TRAIT_H

#include <cassert>
#include <cmath>

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
        assert(t >= 0);
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
        assert(t >= 0);
        s_zero_tol = t;
    }
private:
    static double s_zero_tol;
};

inline float inv(float f)
{
    assert(f);
    return 1.0f / f;
}

inline double inv(double d)
{
    assert(d);
    return 1.0 / d;
}

template<typename T>
inline bool is_zero(T v, T tol = math_trait<T>::zero_tolerance())
{
    using std::abs; // for ADL
    return abs(v) < tol;
}

} // namespace math

} // namespace kismet

#endif // KISMET_MATH_TRAIT_H
