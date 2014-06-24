#ifndef KISMET_MATH_LINEAR_SYSTEM_H
#define KISMET_MATH_LINEAR_SYSTEM_H

#include <cmath>
#include "kismet/config.h"
#include "kismet/utility.h"
#include "kismet/math/math_trait.h"
#include "kismet/math/matrix.h"

namespace kismet
{

namespace math
{

/// Solve a 2x2 linear system, output the result to it
/// ax = b, x is output to it
/// Return true if successful, otherwise return false
template<typename T, typename Out>
bool solve(T const a[2][2], T const b[2], Out it, T tolerance = math_trait<T>::zero_tolerance())
{
    // Calculate the determinant of a
    T det = a[0][0] * a[1][1] - a[1][0] * a[0][1];

    // Determinant too small, no unique solution
    if (is_zero(det, tolerance))
        return false;

    T inv_det = inv(det);

    *it++ = (b[0] * a[1][1] - b[1] * a[0][1]) * inv_det;
    *it   = (a[0][0] * b[1] - a[1][0] * b[0]) * inv_det;

    return true;
}

/// Solve a 3x3 linear system
/// Return true if successful, otherwise return false
template<typename T, typename Out>
bool solve(T const a[3][3], T const b[3], Out it, T tolerance = math_trait<T>::zero_tolerance())
{
    T inv_mat[3][3];
    // Calculate the adjoint matrix
    inv_mat[0][0] = a[1][1] * a[2][2] - a[1][2] * a[2][1];
    inv_mat[0][1] = a[0][2] * a[2][1] - a[0][1] * a[2][2];
    inv_mat[0][2] = a[0][1] * a[1][2] - a[0][2] * a[1][1];
    inv_mat[1][0] = a[1][2] * a[2][0] - a[1][0] * a[2][2];
    inv_mat[1][1] = a[0][0] * a[2][2] - a[0][2] * a[2][0];
    inv_mat[1][2] = a[0][2] * a[1][0] - a[0][0] * a[1][2];
    inv_mat[2][0] = a[1][0] * a[2][1] - a[1][1] * a[2][0];
    inv_mat[2][1] = a[0][1] * a[2][0] - a[0][0] * a[2][1];
    inv_mat[2][2] = a[0][0] * a[1][1] - a[0][1] * a[2][0];

    // Calculate the determinant of a
    T det = a[0][0] * inv_mat[0][0] + a[0][1] * inv_mat[1][0] + a[0][2] * inv_mat[2][0];

    // Determinant too small, no unique solution
    if (is_zero(det, tolerance))
        return false;

    T inv_det = inv(det);

    *it++ = (b[0] * inv_mat[0][0] + b[1] * inv_mat[0][1] + b[2] * inv_mat[0][2]) * inv_det;
    *it++ = (b[0] * inv_mat[1][0] + b[1] * inv_mat[1][1] + b[2] * inv_mat[1][2]) * inv_det;
    *it   = (b[0] * inv_mat[2][0] + b[1] * inv_mat[2][1] + b[2] * inv_mat[2][2]) * inv_det;

    return true;
}

/// solve a linear system ax = b using Gaussian elimination with partial pivoting
/// a is coefficient matrix
/// b is constant matrix
/// it is an random access iterator
template<typename T, std::size_t N, typename RandIt>
bool solve_partial_pivoting(matrix<T, N, N> a, matrix<T, N, 1> b, RandIt it, T tolerance = math_trait<T>::zero_tolerance())
{
    using std::size_t;
    using std::abs;

    // elimination process
    for (size_t i = 0; i < N - 1; ++i)
    {
        // find the pivot in column i
        T pivot = abs(a[i][i]);

        size_t pivot_row = i;
        for (size_t j = i + 1; j < N; ++j)
        {
            T value = abs(a[j][i]);
            if (value > pivot)
            {
                pivot = value;
                pivot_row = j;
            }
        }

        // a is non-invertible, fail
        if (is_zero(pivot, tolerance))
            return false;

        // pivoting
        if (pivot_row != i)
        {
            a[pivot_row].swap(a[i]);
            b[pivot_row].swap(b[i]);
        }

        T neg_inv_pivot = -inv(a[i][i]);

        // eliminate c[i + 1..N][i].
        // actually, we don't bother setting a[row][i] to 0,
        // as we will never use this element in back substitution.
        for (size_t row = i + 1; row < N; ++row)
        {
            T inv_scale = neg_inv_pivot * a[row][i];
            for (size_t col = i + 1; col < N; ++col)
            {
                a[row][col] += inv_scale * a[i][col];
            }

            b[row] += b[i] * inv_scale;
        }
    }

    // non-invertible, fail
    if (is_zero(a[N - 1][N - 1], tolerance))
        return false;

    // back substitution
    for (size_t row = N; row--;)
    {
        for (size_t col = N - 1; col > row; --col)
        {
            b[row] -= a[row][col] * it[col];
        }

        *(it + row) = b[row] * inv(a[row][row]);
    }

    return true;
}

/// LU decompose a matrix with Gaussian Elimination.
/// The matrix A is decomposed as
///     A = L*U
/// where L is a lower triangular matrix, U is a upper triangular matrix
/// Return true if decomposition succeeds
template<typename T, std::size_t N>
bool lu_decompose(matrix<T, N, N> const& a, matrix<T, N, N>& l, matrix<T, N, N>& u, T tolerance = math_trait<T>::zero_tolerance())
{
    using std::size_t;
    using std::abs;

    // we do Gaussian Elimination on u, so make a copy first
    u = a;

    // l starts with a identity matrix
    l = matrix<T, N, N>::identity();

    // elimination process
    for (size_t i = 0; i < N - 1; ++i)
    {
        // no pivoting
        T pivot = u[i][i];

        // check if any other elements in this column is non-zero
        // if so, then we cannot continue decomposing, otherwise
        // we can continue without problems
        if (is_zero(pivot, tolerance))
        {
            auto colv = u.col(i);
            if (!std::all_of(
                    colv.begin() + (i + 1), colv.end(),
                    [tolerance](T const& v) { return is_zero(v, tolerance); }))
            {
                return false;
            }

            continue;
        }

        T neg_inv_pivot = -inv(pivot);

        // eliminate c[i + 1..N][i].
        // we need to set u[row][i] to 0, as we do Gaussian Elimination on u.
        for (size_t row = i + 1; row < N; ++row)
        {
            T inv_scale = neg_inv_pivot * u[row][i];

            // zero, as it's eliminated.
            u[row][i] = (T)0;

            // set the corresponding entry in lower triangular matrix
            l[row][i] = -inv_scale;

            for (size_t col = i + 1; col < N; ++col)
            {
                u[row][col] += inv_scale * u[i][col];
            }
        }
    }

    return true;
}

    return true;
}
KISMET_FUNC_TEMPLATE_API(solve, bool, float const a[2][2], float const b[2], float* it, float tol)
KISMET_FUNC_TEMPLATE_API(solve, bool, double const a[2][2], double const b[2], double* it, double tol)

KISMET_FUNC_TEMPLATE_API(solve, bool, float const a[3][3], float const b[3], float* it, float tol)
KISMET_FUNC_TEMPLATE_API(solve, bool, double const a[3][3], double const b[3], double* it, double tol)

} // namespace math

} // namespace kismet

#endif // KISMET_MATH_LINEAR_SYSTEM_H
