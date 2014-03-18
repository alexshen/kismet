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

/// Solve a 2x2 linear system, output the result to it
/// ax = b, x is output to it
/// Return true if successful, otherwise return false
template<typename T, typename Out>
bool solve(T const a[2][2], T const b[2], Out it, T tolerance = math_trait<T>::zero_tolerance())
{
    using std::abs; // for ADL

    // Calculate the determinant of a
    T det = a[0][0] * a[1][1] - a[1][0] * a[0][1];

    // Determinant too small, no unique solution
    if (abs(det) < tolerance)
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
    using std::abs; // for ADL

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
    if (abs(det) < tolerance)
        return false;

    T inv_det = inv(det);

    *it++ = (b[0] * inv_mat[0][0] + b[1] * inv_mat[0][1] + b[2] * inv_mat[0][2]) * inv_det;
    *it++ = (b[0] * inv_mat[1][0] + b[1] * inv_mat[1][1] + b[2] * inv_mat[1][2]) * inv_det;
    *it   = (b[0] * inv_mat[2][0] + b[1] * inv_mat[2][1] + b[2] * inv_mat[2][2]) * inv_det;

    return true;
}

KISMET_FUNC_TEMPLATE_API(solve, bool, float const a[2][2], float const b[2], float* it, float tol)
KISMET_FUNC_TEMPLATE_API(solve, double const a[2][2], double const b[2], double* it, double tol)

KISMET_FUNC_TEMPLATE_API(solve, bool, float const a[3][3], float const b[3], float* it, float tol)
KISMET_FUNC_TEMPLATE_API(solve, bool, double const a[3][3], double const b[3], double* it, double tol)

} // namespace math

} // namespace kismet

#endif // KISMET_MATH_LINEAR_SYSTEM_H
