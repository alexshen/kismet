#ifndef KISMET_MATH_LINEAR_SYSTEM_H
#define KISMET_MATH_LINEAR_SYSTEM_H

#include <cmath>
#include "kismet/config.h"
#include "kismet/utility.h"
#include "kismet/math/math_trait.h"

namespace kismet
{

namespace math
{

// forward declaration
template<typename T, std::size_t N1, std::size_t N2>
class matrix;

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
    inv_mat[2][2] = a[0][0] * a[1][1] - a[0][1] * a[1][0];

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

namespace detail
{

template<bool CheckZero, typename T, std::size_t N, typename RandIt>
bool backward_substitute_impl(matrix<T, N, N> const& a, matrix<T, N, 1> const& b, RandIt x, T tolerance)
{
    // back substitution
    for (std::size_t row = N; row--;)
    {
        T coff = a[row][row];
        // If the upper triangular matrix is non invertible
        // return false
        if (CheckZero && is_zero(coff, tolerance))
        {
            return false;
        }

        KISMET_ASSERT(!is_zero(coff, tolerance));

        T v(b[row]);
        for (std::size_t col = N - 1; col > row; --col)
        {
            v -= a[row][col] * x[col];
        }

        x[row] = v * inv(coff);
    }

    return true;
}

} // namespace detail

/// Backward substitution
///   A*x=b
/// A is a upper triangular matrix
/// Return false if substitution failure
template<typename T, std::size_t N>
bool backward_substitute(matrix<T, N, N> const& a, matrix<T, N, 1> const& b, matrix<T, N, 1>& x, T tolerance = math_trait<T>::zero_tolerance())
{
    return detail::backward_substitute_impl<true>(a, b, x.data(), tolerance);
}

template<typename T, std::size_t N, typename RandIt>
bool backward_substitute(matrix<T, N, N> const& a, matrix<T, N, 1> const& b, RandIt x, T tolerance = math_trait<T>::zero_tolerance())
{
    return detail::backward_substitute_impl<true>(a, b, x, tolerance);
}

/// Backward substitution
///   A*x=b
/// A is a lower triangular matrix
/// Return false if substitution failure
template<typename T, std::size_t N, typename RandIt>
bool forward_substitute(matrix<T, N, N> const& a, matrix<T, N, 1> const& b, RandIt x, T tolerance = math_trait<T>::zero_tolerance())
{
    for (std::size_t row = 0; row < N; ++row)
    {
        T coff = a[row][row];

        // If the lower triangular matrix is non invertible
        // return false
        if (is_zero(coff, tolerance))
        {
            return false;
        }

        T v(b[row]);
        for (std::size_t col = 0; col < row; ++col)
        {
            v -= a[row][col] * x[col];
        }

        x[row] = v * inv(coff);
    }

    return true;
}

template<typename T, std::size_t N>
bool forward_substitute(matrix<T, N, N> const& a, matrix<T, N, 1> const& b, matrix<T, N, 1>& x, T tolerance = math_trait<T>::zero_tolerance())
{
    return forward_substitute(a, b, x.data(), tolerance);
}

/// solve a linear system ax = b using Gaussian elimination with partial pivoting
/// a is coefficient matrix
/// b is constant matrix
template<typename T, std::size_t N>
bool solve_partial_pivoting(matrix<T, N, N> a, matrix<T, N, 1> b, matrix<T, N, 1>& x, T tolerance = math_trait<T>::zero_tolerance())
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
    // No check for convertibility, as we have check in the above.
    detail::backward_substitute_impl<false>(a, b, x.data(), tolerance);

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
            auto colv = u.column(i);
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
            u[row][i] = T(0);

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

namespace detail
{

// Decompose the matrix as
//   A = L'*U
// Record all permutations when doing Gaussian Elimination in P
// The lower triangular matrix should be constructed as
//   L = P * L'
template<typename T, std::size_t N>
void plu_decompose_helper(matrix<T, N, N> const& a, std::size_t (&perms) [N], matrix<T, N, N>& l, matrix<T, N, N>& u, T tolerance)
{
    using std::size_t;
    using std::abs;
    using std::swap;

    // array form of permutation which records the permutation
    // when doing Gaussian Elimination.
    // the identity element's index is (i, p[i])
    for (size_t i = 0; i < N; ++i)
    {
        perms[i] = i;
    }

    // we do Gaussian Elimination on u, so make a copy first
    u = a;

    // lower triangular matrix starts with identity
    l = matrix<T, N, N>::identity();

    // elimination process
    for (size_t i = 0; i < N - 1; ++i)
    {
        // find the pivot in column i
        T pivot = abs(u[i][i]);

        size_t pivot_row = i;
        for (size_t j = i + 1; j < N; ++j)
        {
            T value = abs(u[j][i]);
            if (value > pivot)
            {
                pivot = value;
                pivot_row = j;
            }
        }

        // pivot is 0, no need to eliminate for this column
        if (is_zero(pivot, tolerance))
        {
            continue;
        }

        // pivoting
        if (pivot_row != i)
        {
            u[pivot_row].swap(u[i]);

            // L'*A = U
            // where L' = Mn-1*Pn-1*...*M1*P1
            // so A = L'^-1*U
            // L'^-1 = P1^-1*M^-1*...*Pn-1^-1*Mn-1^-1
            // as Pi is the an elementary matrix of row operation, so its
            // inverse is an elementary matrix of column operation.
            // Also we have L'^-1 = P1^T*M^-1*...*Pn-1^T*Mn-1^-1

            // swap columns which indices are `i' and `pivot_row'
            // we only need to swap the corresponding identity elements of elementary operation

            // the column of the identity elements on the left side
            size_t icol = perms[i];
            size_t pivot_col = perms[pivot_row];

            // permutation matrix's inverse equals to its transpose
            swap(l[icol][i], l[icol][pivot_row]);
            swap(l[pivot_col][i], l[pivot_col][pivot_row]);

            // save the permutation
            swap(perms[i], perms[pivot_row]);
        }

        T neg_inv_pivot = -inv(u[i][i]);

        // eliminate c[i + 1..N][i].
        // we need to set u[row][i] to 0, as we do Gaussian Elimination on u.
        for (size_t row = i + 1; row < N; ++row)
        {
            T inv_scale = neg_inv_pivot * u[row][i];

            // zero, as it's eliminated.
            u[row][i] = T(0);

            // apply the column operation to i-th column of the lower triangular matrix
            // there's only one identity element in the row-th column of the lower triangular matrix
            l[perms[row]][i] -= inv_scale;

            for (size_t col = i + 1; col < N; ++col)
            {
                u[row][col] += inv_scale * u[i][col];
            }
        }
    }
}

} // namespace detail

/// PLU decompose a matrix with Gaussian Elimination.
/// The matrix A is decomposed as
///     A = P*L*U
/// where P is a permutation matrix, L is a lower triangular matrix, U is a upper triangular matrix
template<typename T, std::size_t N>
void plu_decompose(matrix<T, N, N> const& a, matrix<T, N, N>& p, matrix<T, N, N>& l, matrix<T, N, N>& u, T tolerance = math_trait<T>::zero_tolerance())
{
    std::size_t perms[N];

    detail::plu_decompose_helper(a, perms, l, u, tolerance);

    // update the permutation matrix
    // for now, p is on the left side
    // P*A = L'*U
    // to get the correct permutation matrix, we need to transpose,
    // as P^-1 = P^T
    p.clear();
    for (int i = 0; i < N; ++i)
    {
        p[perms[i]][i] = T(1);
    }

    // apply permutation to the almost lower triangular matrix 
    // to get the real lower triangular matrix
    reorder(l.row_begin(), l.row_end(), perms);
}

/// PLU decompose a matrix with Gaussian Elimination.
/// The matrix A is decomposed as
///     A = P*L*U
/// where P is an array which represents a permutation matrix, L is a lower triangular matrix, U is a upper triangular matrix
template<typename T, std::size_t N>
void plu_decompose(matrix<T, N, N> const& a, std::size_t (&p) [N], matrix<T, N, N>& l, matrix<T, N, N>& u, T tolerance = math_trait<T>::zero_tolerance())
{
    std::size_t perms[N];

    detail::plu_decompose_helper(a, perms, l, u, tolerance);

    // update the permutation matrix
    // for now, p is on the left side
    // P*A = L'*U
    // to get the correct permutation matrix, we need to transpose,
    // as P^-1 = P^T
    for (int i = 0; i < N; ++i)
    {
        p[perms[i]] = i;
    }

    // apply permutation to the almost lower triangular matrix 
    // to get the real lower triangular matrix
    reorder(l.row_begin(), l.row_end(), perms);
}

KISMET_FUNC_TEMPLATE_API(solve, bool, float const a[2][2], float const b[2], float* it, float tol)
KISMET_FUNC_TEMPLATE_API(solve, bool, double const a[2][2], double const b[2], double* it, double tol)

KISMET_FUNC_TEMPLATE_API(solve, bool, float const a[3][3], float const b[3], float* it, float tol)
KISMET_FUNC_TEMPLATE_API(solve, bool, double const a[3][3], double const b[3], double* it, double tol)

} // namespace math

} // namespace kismet

#endif // KISMET_MATH_LINEAR_SYSTEM_H
