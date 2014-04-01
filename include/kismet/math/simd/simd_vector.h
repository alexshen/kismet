#ifndef KISMET_MATH_SIMD_VECTOR_H
#define KISMET_MATH_SIMD_VECTOR_H

#include <algorithm>
#include <cstddef>
#include <xmmintrin.h>
#include <utility>
#include "kismet/config.h"

namespace kismet
{
namespace math
{
namespace simd
{

template<typename T>
struct vector
{
    // each specialization must define this as true
    static const bool is_specialized = false;
    static const std::size_t alignment = 1;
};

template<>
struct vector<float>
{
    static const bool is_specialized = true;
    static const std::size_t alignment = 16;
};

namespace detail
{

template<typename T, std::size_t N>
struct scale_impl
{
    static void scale(T const* v, T k, T* o) {}
};

template<>
struct scale_impl<float, 1>
{
    static void scale(float const* v, float k, float* o)
    {
        *o = *v * k;
    }
};

template<>
struct scale_impl<float, 2>
{
    static void scale(float const* v, float k, float* o)
    {
        __m128 a = _mm_set_ps(0.f, *(v+2), *(v+1), *v);
        __m128 b = _mm_set_ps1(k);
        a = _mm_mul_ps(a, b);
        _mm_store_ss(o, a);
        a = _mm_shuffle_ps(a, a, 0x35);
        _mm_store_ss(o + 1, a);
    }
};

template<>
struct scale_impl<float, 3>
{
    static void scale(float const* v, float k, float* o)
    {
        __m128 a = _mm_set_ps(0.f, *(v+2), *(v+1), *v);
        __m128 b = _mm_set_ps1(k);
        a = _mm_mul_ps(a, b);
        _mm_store_ss(o, a);
        a = _mm_shuffle_ps(a, a, 0x35);
        _mm_store_ss(o + 1, a);
        a = _mm_shuffle_ps(a, a, 0x35);
        _mm_store_ss(o + 2, a);
    }
};

template<>
struct scale_impl<float, 4>
{
    static void scale(float const* v, float k, float* o)
    {
        __m128 a = _mm_load_ps(v);
        __m128 b = _mm_set_ps1(k);
        _mm_store_ps(o, _mm_mul_ps(a, b));
    }
};

template<typename T, std::size_t N>
struct scale_loop_impl
{
    static int go(T const* v, T k, T* o)
    {
    }
};

template<typename T, std::size_t N, bool less_than_4>
struct scale_loop
{
    static void go(T const* v, T k, T* o)
    {
        scale_impl<T, 4>::scale(v, k, o);
        scale_loop<T, N-4, (N<4)>::go(v + 4, k, o + 4);
    }
};

template<typename T, std::size_t N>
struct scale_loop<T, N, true>
{
    static void go(T const* v, T k, T* o)
    {
        scale_impl<T, N>::scale(v, k, o);
    }
};

template<std::size_t N>
inline void scale(float const* v, float k, float* o, std::true_type)
{
    scale_loop<float, N, (N<4)>::go(v, k, o);
}

template<std::size_t N, typename T>
inline void scale(T const* v, T k, T* o, std::false_type)
{
    std::transform(v, v + N, o, [k](T a) { return a * k; });
}

} // namespace detail

template<std::size_t N, typename T>
inline void scale(T const* v, T k, T* o)
{
    KISMET_ASSERT_ALIGN(v, vector<T>::alignment);
    KISMET_ASSERT_ALIGN(o, vector<T>::alignment);
    detail::scale<N>(v, k, o, std::integral_constant<bool, vector<T>::is_specialized>());
}

} // namespace simd
} // namespace math
} // namespace kismet

#endif // KISMET_MATH_SIMD_VECTOR_H
