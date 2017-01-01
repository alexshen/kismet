#ifndef KISMET_MATH_TRANSFORM_H
#define KISMET_MATH_TRANSFORM_H

#include "kismet/core/assert.h"

#include "kismet/math/matrix.h"
#include "kismet/math/quaternion.h"
#include "kismet/math/utility.h"
#include "kismet/math/vector.h"

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

// convert rotation matrix to quaternion
template<typename T>
quaternion<T> matrix_to_quat(matrix44<T> const& m)
{
    quaternion<T> res;
    // trace = 4w^2 - 1
    T trace = m[0][0] + m[1][1] + m[2][2];
    if (trace > T(0))
    {
        res.w() = std::sqrt(trace + T(1)) * T(0.5);
        T inv_denom = T(0.25) / res.w();
        res.x() = inv_denom * (m[2][1] - m[1][2]);
        res.y() = inv_denom * (m[0][2] - m[2][0]);
        res.z() = inv_denom * (m[1][0] - m[0][1]);
    }
    else
    {
        int i = max_index(m[0][0], m[1][1], m[2][2]);
        if (i == 0)
        {
            res.x() = std::sqrt(T(1) + m[0][0] - m[1][1] - m[2][2]) * T(0.5);
            T inv_denom = T(0.25) / res.x();
            res.y() = inv_denom * (m[0][1] + m[1][0]);
            res.z() = inv_denom * (m[0][2] + m[2][0]);
            res.w() = inv_denom * (m[2][1] - m[1][2]);
        }
        else if (i == 1)
        {
            res.y() = std::sqrt(T(1) - m[0][0] + m[1][1] - m[2][2]) * T(0.5);
            T inv_denom = T(0.25) / res.y();
            res.x() = inv_denom * (m[0][1] + m[1][0]);
            res.z() = inv_denom * (m[1][2] + m[2][1]);
            res.w() = inv_denom * (m[0][2] - m[2][0]);
        }
        else
        {
            res.z() = std::sqrt(T(1) - m[0][0] - m[1][1] + m[2][2]) * T(0.5);
            T inv_denom = T(0.25) * res.z();
            res.x() = inv_denom * (m[0][2] + m[2][0]);
            res.y() = inv_denom * (m[1][2] + m[2][1]);
            res.w() = inv_denom * (m[1][0] - m[0][1]);
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
inline matrix44<T> euler_yxz_to_matrix(vector3<T> const& e)
{
    return euler_yxz_to_matrix(e.y(), e.x(), e.z());
}

template<typename T>
inline matrix44<T> matrix_scale(T sx, T sy, T sz)
{
    matrix44<T> res = matrix44<T>::identity;
    res[0][0] = sx;
    res[1][1] = sy;
    res[2][2] = sz;
    return res;
}

template<typename T>
inline matrix44<T> matrix_scale(vector3<T> const& s)
{
    return matrix_scale(s.x(), s.y(), s.z());
}

template<typename T>
inline matrix44<T> matrix_scale(T s)
{
    matrix44<T> res = matrix44<T>::identity;
    res[0][0] = s;
    res[1][1] = s;
    res[2][2] = s;
    return res;
}

template<typename T>
inline matrix44<T> matrix_translate(T x, T y, T z)
{
    matrix44<T> res = matrix44<T>::identity;
    res[0][3] = x;
    res[1][3] = y;
    res[2][3] = z;
    return res;
}

template<typename T>
inline matrix44<T> matrix_translate(vector3<T> const& delta)
{
    return matrix_translate(delta.x(), delta.y(), delta.z());
}

// extract scale from the TRS matrix
template<typename T>
inline vector3<T> extract_scale(matrix44<T> const& m)
{
    vector3<T> scale;
    scale.x() = mag(m.column(0));
    scale.y() = mag(m.column(1));
    scale.z() = mag(m.column(2));
    return scale;
}

// extract translation from the TRS matrix
template<typename T>
inline vector3<T> extract_translation(matrix44<T> const& m)
{
    return vector3<T>(m[0][3], m[1][3], m[2][3]);
}

// extract quaternion from the TRS matrix
template<typename T>
inline quaternion<T> extract_quaternion(matrix44<T> const& m)
{
    matrix44<T> tmp(m);
    // normalize the rotation part to remove scaling
    for (std::size_t i = 0; i < 3; ++i)
    {
        tmp.column(i) /= mag(tmp.column(i));
    }
    return matrix_to_quat(tmp);
}

// extract scale and rotation from the TRS matrix
template<typename T>
inline void extract_sr(matrix44<T> const& m, vector3<T>& s, quaternion<T>& q)
{
    extract_scale(m, s);
    matrix44<T> tmp(m);
    for (std::size_t i = 0; i < 3; ++i)
    {
        tmp.column(i) /= s[i];
    }
    extract_quaternion(tmp, q);
}

} // namespace math
} // namespace kismet

#endif // KISMET_MATH_TRANSFORM_H
