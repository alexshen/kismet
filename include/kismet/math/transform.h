#ifndef KISMET_MATH_TRANSFORM_H
#define KISMET_MATH_TRANSFORM_H

#include "kismet/core/assert.h"

#include "kismet/math/quaternion.h"
#include "kismet/math/matrix.h"
#include "kismet/math/vector.h"
#include "kismet/math/utility.h"

#include <cmath>

namespace kismet
{
namespace math
{

template<typename T>
matrix44<T> quat_to_matrix(quaternion<T> const& q)
{
    matrix44<T> res;

    T w2 = q.w() * q.w();
    T xy = q.x() * q.y();
    T wz = q.w() * q.z();
    T xz = q.x() * q.z();
    T wy = q.w() * q.y();
    T yz = q.y() * q.z();
    T wx = q.w() * q.x();

    res[0][0] = T(2) * (w2 + q.x() * q.x()) - T(1);
    res[0][1] = T(2) * (xy - wz);
    res[0][2] = T(2) * (xz + wy);
    res[0][3] = T(0);

    res[1][0] = T(2) * (xy + wz);
    res[1][1] = T(2) * (w2 + q.y() * q.y()) - T(1);
    res[1][2] = T(2) * (yz - wx);
    res[1][3] = T(0);

    res[2][0] = T(2) * (xz - wy);
    res[2][1] = T(2) * (yz + wx);
    res[2][2] = T(2) * (w2 + q.z() * q.z()) - T(1);
    res[2][3] = T(0);

    res[3][0] = res[3][1] = res[3][2] = T(0);
    res[3][3] = T(1);

    return res;
}

template<typename T>
quaternion<T> matrix_to_quat(matrix44<T> const& m)
{
    quaternion<T> res;
    // trace = 4w^2 - 1
    T trace = m[0][0] + m[1][1] + m[2][2];
    if (trace > T(0))
    {
        res.w(std::sqrt(trace + T(1)) * T(0.5));
        T inv_denom = T(0.25) / res.w();
        res.x(inv_denom * (m[2][1] - m[1][2]));
        res.y(inv_denom * (m[0][2] - m[2][0]));
        res.z(inv_denom * (m[1][0] - m[0][1]));
    }
    else
    {
        int i = max_index(m[0][0], m[1][1], m[2][2]);
        if (i == 0)
        {
            res.x(std::sqrt(T(1) + m[0][0] - m[1][1] - m[2][2]) * T(0.5));
            T inv_denom = T(0.25) / res.x();
            res.y(inv_denom * (m[0][1] + m[1][0]));
            res.z(inv_denom * (m[0][2] + m[2][0]));
            res.w(inv_denom * (m[2][1] - m[1][2]));
        }
        else if (i == 1)
        {
            res.y(std::sqrt(T(1) - m[0][0] + m[1][1] - m[2][2]) * T(0.5));
            T inv_denom = T(0.25) / res.y();
            res.x(inv_denom * (m[0][1] + m[1][0]));
            res.z(inv_denom * (m[1][2] + m[2][1]));
            res.w(inv_denom * (m[0][2] - m[2][0]));
        }
        else
        {
            res.z(std::sqrt(T(1) - m[0][0] - m[1][1] + m[2][2]) * T(0.5));
            T inv_denom = T(0.25) * res.z();
            res.x(inv_denom * (m[0][2] + m[2][0]));
            res.y(inv_denom * (m[1][2] + m[2][1]));
            res.w(inv_denom * (m[1][0] - m[0][1]));
        }
    }
    return res;
}

// euler angles to matrix
// the rotation is applied in the order specified as in the function name in the local space

template<typename T>
matrix44<T> euler_yxz_to_matrix(T y, T x, T z)
{
    using std::cos;
    using std::sin;

    matrix44<T> res; 

    T cy = cos(y);
    T cx = cos(x);
    T cz = cos(z);
    T sy = sin(y);
    T sx = sin(x);
    T sz = sin(z);

    T sysx = sy * sx;
    T cysx = cy * sx;

    res[0][0] = cy * cz + sysx * sz;
    res[0][1] = -cy * sz + sysx * cz;
    res[0][2] = sy * cx;
    res[0][3] = T(0);

    res[1][0] = cx * sz;
    res[1][1] = cx * cz;
    res[1][2] = -sx;
    res[1][3] = T(0);

    res[2][0] = -sy * cz + cysx * sz;
    res[2][1] = sy * sz + cysx * cz;
    res[2][2] = cy * cx;
    res[2][3] = T(0);

    res[3][0] = res[3][1] = res[3][2] = T(0);
    res[3][3] = T(1);

    return res;
}

template<typename T>
inline matrix44<T> matrix_scale44(T s)
{
    matrix44<T> res = matrix44<T>::identity;
    res[0][0] = s;
    res[1][1] = s;
    res[2][2] = s;
    return res;
}

template<typename T>
inline matrix44<T> matrix_translate(vector<T, 3> const& delta)
{
    matrix44<T> res = matrix44<T>::identity;
    res[0][3] = delta.x();
    res[1][3] = delta.y();
    res[2][3] = delta.z();
    return res;
}

} // namespace math
} // namespace kismet

#endif // KISMET_MATH_TRANSFORM_H
