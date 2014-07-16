#ifndef KISMET_MATH_MATRIX_MUL_H
#define KISMET_MATH_MATRIX_MUL_H

#include <cstddef>
#include <numeric>
#include <type_traits>

namespace kismet
{
namespace math
{

template<typename T, std::size_t N, std::size_t S>
class matrix_vector;

template<typename T, std::size_t N1, std::size_t N2>
class matrix;

namespace detail
{

int const loop_unroll_limit = 4;

template<typename T, std::size_t N2, std::size_t N3>
inline T row_mul_column(matrix_vector<T, N2, 1> const& v1, matrix_vector<T, N2, N3> const& v2)
{
    return std::inner_product(v1.begin(), v1.end(), v2.begin(), T(0));
}

template<typename T, std::size_t N2, std::size_t N3, std::size_t I>
struct matrix_mul_inner_loop
{
    static void calc(matrix_vector<T const, N2, 1> const& row, matrix<T, N2, N3> const& m2, matrix_vector<T, N3, 1>& res_row)
    {
        res_row[I] = row_mul_column(row, m2.column(I));
        matrix_mul_inner_loop<T, N2, N3, I + 1>::calc(row, m2, res_row);
    }
};

template<typename T, std::size_t N2, std::size_t N3>
struct matrix_mul_inner_loop<T, N2, N3, N3>
{
    static void calc(matrix_vector<T const, N2, 1> const& row, matrix<T, N2, N3> const& m2, matrix_vector<T, N3, 1>& res_row) {}
};

template<typename T, std::size_t N2, std::size_t N3>
inline void row_mul_matrix(matrix_vector<T const, N2, 1> const& row, matrix<T, N2, N3> const& m2, matrix_vector<T, N3, 1>& res_row, std::true_type)
{
    matrix_mul_inner_loop<T, N2, N3, 0>::calc(row, m2, res_row);
}

template<typename T, std::size_t N2, std::size_t N3>
inline void row_mul_matrix(matrix_vector<T const, N2, 1> const& row, matrix<T, N2, N3> const& m2, matrix_vector<T, N3, 1>& res_row, std::false_type)
{
    for (std::size_t i = 0; i < N3; ++i)
    {
        res_row[i] = row_mul_column(row, m2.column(i));
    }
}

template<typename T, std::size_t N2, std::size_t N3>
inline void row_mul_matrix(matrix_vector<T const, N2, 1> const& row, matrix<T, N2, N3> const& m2, matrix_vector<T, N3, 1>& res_row)
{
    row_mul_matrix(row, m2, res_row, std::integral_constant<bool, N3 <= loop_unroll_limit>());
}

template<typename T, std::size_t N1, std::size_t N2, std::size_t N3, std::size_t I>
struct matrix_mul_outer_loop
{
    static void calc(matrix<T, N1, N2> const& m1, matrix<T, N2, N3> const& m2, matrix<T, N1, N3>& res)
    {
        auto res_row = res.row(I);
        row_mul_matrix(m1.row(I), m2, res_row);
        matrix_mul_outer_loop<T, N1, N2, N3, I + 1>::calc(m1, m2, res);
    }
};

template<typename T, std::size_t N1, std::size_t N2, std::size_t N3>
struct matrix_mul_outer_loop<T, N1, N2, N3, N1>
{
    static void calc(matrix<T, N1, N2> const& m1, matrix<T, N2, N3> const& m2, matrix<T, N1, N3>& res) {}
};

template<typename T, std::size_t N1, std::size_t N2, std::size_t N3>
inline matrix<T, N1, N3> matrix_mul(matrix<T, N1, N2> const& m1, matrix<T, N2, N3> const& m2, std::true_type)
{
    matrix<T, N1, N3> res;
    matrix_mul_outer_loop<T, N1, N2, N3, 0>::calc(m1, m2, res);
    return res;
}

template<typename T, std::size_t N1, std::size_t N2, std::size_t N3>
inline matrix<T, N1, N3> matrix_mul(matrix<T, N1, N2> const& m1, matrix<T, N2, N3> const& m2, std::false_type)
{
    matrix<T, N1, N3> res;
    for (std::size_t i = 0; i < N1; ++i)
    {
        auto res_row = res.row(i);
        row_mul_matrix(m1.row(i), m2, res_row);
    }
    return res;
}

template<typename T, std::size_t N1, std::size_t N2, std::size_t N3>
inline matrix<T, N1, N3> matrix_mul(matrix<T, N1, N2> const& m1, matrix<T, N2, N3> const& m2)
{
    return matrix_mul(m1, m2, std::integral_constant<bool, N1 <= loop_unroll_limit>());
}

} // namespace detail
} // namespace math
} // namespace kismet

#endif // KISMET_MATH_MATRIX_MUL_H
