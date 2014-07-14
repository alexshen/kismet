#ifndef KISMET_MATH_MATRIX_MUL_H
#define KISMET_MATH_MATRIX_MUL_H

#include <cstddef>
#include <numeric>

namespace kismet
{
namespace math
{

template<typename T, std::size_t N1, std::size_t N2>
class matrix;

namespace detail
{

template<typename T, std::size_t N1, std::size_t N2, std::size_t I>
struct left_mul_loop
{
    static void calc(T const* v, matrix<T, N1, N2> const& m, T* u)
    {
        auto col = m.column(I);
        *u = std::inner_product(col.begin(), col.end(), v, T(0));
        left_mul_loop<T, N1, N2, I + 1>::calc(v, m, u + 1);
    }
};

template<typename T, std::size_t N1, std::size_t N2>
struct left_mul_loop<T, N1, N2, N2>
{
    static void calc(T const* v, matrix<T, N1, N2> const& m, T* u) {}
};

// u = v * M
// left multiply a vector with a matrix
template<typename T, std::size_t N1, std::size_t N2>
inline void mul(T const* v, matrix<T, N1, N2> const& m, T* u)
{
    left_mul_loop<T, N1, N2, 0>::calc(v, m, u);
}

template<typename T, std::size_t N1, std::size_t N2, std::size_t I>
struct right_mul_loop
{
    static void calc(matrix<T, N1, N2> const& m, T const* v, T* u)
    {
        auto row = m.row(I);
        *u = std::inner_product(row.begin(), row.end(), v, T(0));
        right_mul_loop<T, N1, N2, I + 1>::calc(m, v, u + 1);
    }
};

template<typename T, std::size_t N1, std::size_t N2>
struct right_mul_loop<T, N1, N2, N1>
{
    static void calc(matrix<T, N1, N2> const& m, T const* v, T* u) {}
};

// u = M * v
// right multiply a vector with a matrix
template<typename T, std::size_t N1, std::size_t N2>
inline void mul(matrix<T, N1, N2> const& m, T const* v, T* u)
{
    right_mul_loop<T, N1, N2, 0>::calc(m, v, u);
}

} // namespace detail
} // namespace math
} // namespace kismet

#endif // KISMET_MATH_MATRIX_MUL_H
