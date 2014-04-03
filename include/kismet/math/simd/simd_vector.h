#ifndef KISMET_MATH_SIMD_VECTOR_H
#define KISMET_MATH_SIMD_VECTOR_H

#include <cstddef>
#include <xmmintrin.h>
#include <utility>
#include <boost/mpl/vector_c.hpp>

namespace kismet
{
namespace math
{
namespace simd
{

template<typename T, std::size_t N>
struct vector
{
    // each specialization must define this as true
    static const bool is_specialized = false;
    static const std::size_t alignment = 1;
};

template<>
struct vector<float, 4>
{
    static const bool is_specialized = false;
    static const std::size_t alignment = 16;

    __m128 d;
};

template<typename T>
struct valid_vector : boost::mpl::vector_c<std::size_t>
{
};

template<>
struct valid_vector<float> : boost::mpl::vector_c<std::size_t, 4>
{
};

namespace detail
{

template<bool, typename It>
struct get_closest_vector_type_impl
{
};

template<typename T, std::size_t N>
struct get_closest_vector_type
{
    using iter = typename boost::mpl::upper_bound<
                            valid_vector<T>,
                            boost::mpl::int_<N>>::type;
    static const int size = boost::mpl::size<valid_vector<T>>::value;
    static const int index = boost::mpl::distance<
        typename boost::mpl::begin<valid_vector<T>>::type,
        iter>::value;
    static const bool found = index < size;
};

template<typename T, std::size_t N>
inline void scale(T const* v, float k, T* output, std::true_type)
{
}

template<typename T, std::size_t N>
inline void scale(T const* v, float k, T* output, std::false_type)
{
}
}

template<typename T, std::size_t N>
inline void scale(T const* v, float k, T* o)
{
    detail::scale(v, k, o, );
}

} // namespace simd
} // namespace math
} // namespace kismet

#endif // KISMET_MATH_SIMD_VECTOR_H
