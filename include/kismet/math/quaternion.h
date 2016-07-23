#ifndef KIMSET_MATH_QUATERNION_H
#define KIMSET_MATH_QUATERNION_H

#include "kismet/math/math_trait.h"
#include "kismet/math/utility.h"
#include "kismet/math/vector.h"

#include <algorithm>
#include <ostream>

#include <cmath>

namespace kismet
{
namespace math
{

template<typename T>
class quaternion
{
public:
    // default construction, not initialized
    quaternion() = default;

    quaternion(T w, T x, T y, T z)
    {
        set(w, x, y, z);
    }

    // create a quaternion from axis angle
    // axis must be a unit vector.
    quaternion(vector3<T> const& v, T angle)
    {
        KISMET_ASSERT(approx(v.mag(), T(1.0)));

        using std::cos;
        using std::sin;

        T ha = angle * T(0.5);
        T sin_ha = sin(ha);

        set(cos(ha), v.x() * sin_ha, v.y() * sin_ha, v.z() * sin_ha);
    }

    T mag() const
    {
        return std::sqrt(squared_mag());
    }

    T squared_mag() const
    {
        return v[0] * v[0] + v[1] * v[1] + v[2] * v[2] + v[3] * v[3];
    }

    void normalize(T tolerance = math_trait<T>::zero_tolerance())
    {
        KISMET_ASSERT(!is_zero(mag(), tolerance));
        *this /= mag();
    }

    // safe normalize the quaternion
    bool safe_normalize(T tolerance = math_trait<T>::zero_tolerance())
    {
        T len = mag();
        if (is_zero(len, tolerance))
        {
            return false;
        }

        *this /= len;
        return true;
    }

    // set the components
    void set(T w, T x, T y, T z)
    {
        v[0] = w;
        v[1] = x;
        v[2] = y;
        v[3] = z;
    }

    // invert the quaternion
    bool invert(T tolerance = math_trait<T>::zero_tolerance())
    {
        T sm = squared_mag();
        if (is_zero(sm))
        {
            return false;
        }

        *this /= sm;
        return true;
    }

    // component mutators
    quaternion& w(T a) { v[0] = a; return *this; }
    quaternion& x(T a) { v[1] = a; return *this; }
    quaternion& y(T a) { v[2] = a; return *this; }
    quaternion& z(T a) { v[3] = a; return *this; }

    // component accessors
    T w() const { return v[0]; }
    T x() const { return v[1]; }
    T y() const { return v[2]; }
    T z() const { return v[3]; }

    T* data() { return v; }
    T const* data() const { return v; }

    // return the dot product of two quaternion
    T dot(quaternion const& q) const
    {
        return v[0] * q.v[0] + v[1] * q.v[1] + v[2] * q.v[2] + v[3] * q.v[3];
    }

    quaternion operator -() const
    {
        quaternion q;
        q.v[0] = -v[0];
        q.v[1] = -v[1];
        q.v[2] = -v[2];
        q.v[3] = -v[3];
        return q;
    }

    quaternion& operator +=(quaternion const& rhs)
    {
        v[0] += rhs.v[0];
        v[1] += rhs.v[1];
        v[2] += rhs.v[2];
        v[3] += rhs.v[3];
        return *this;
    }

    quaternion& operator -=(quaternion const& rhs)
    {
        v[0] -= rhs.v[0];
        v[1] -= rhs.v[1];
        v[2] -= rhs.v[2];
        v[3] -= rhs.v[3];
        return *this;
    }

    quaternion& operator *=(T scale)
    {
        v[0] *= scale;
        v[1] *= scale;
        v[2] *= scale;
        v[3] *= scale;
        return *this;
    }

    // left multiply a quaternion
    quaternion& operator *=(quaternion const& rhs)
    {
        T w0 = w() * rhs.w() - x() * rhs.x() - y() * rhs.y() - z() * rhs.z();
        T x0 = w() * rhs.x() - rhs.w() * x() + y() * rhs.z() - z() * rhs.y();
        T y0 = w() * rhs.y() - rhs.w() * y() + z() * rhs.x() - x() * rhs.z();
        T z0 = w() * rhs.z() - rhs.w() * z() + x() * rhs.y() - y() * rhs.x();

        w(w0).x(x0).y(y0).z(z0);
        return *this;
    }

    quaternion& operator /=(T scale)
    {
        KISMET_ASSERT(!is_zero(scale));
        return this *= invert(scale);
    }

    // w: 0, x: 1, y: 2, z:3
    T& operator [](std::size_t index)
    {
        KISMET_ASSERT(index < 4);
        return v[index];
    }

    T operator [](std::size_t index) const
    {
        KISMET_ASSERT(index < 4);
        return v[index];
    }

    // The identity quaternion
    static quaternion const identity;
private:
    T v[4];
};

template<typename T>
quaternion<T> const quaternion<T>::identity( T( 1.0 ), T(), T(), T() );

// return the magnitude of the quaternion
template<typename T>
inline T mag(quaternion<T> const& q)
{
    return q.mag();
}

template<typename T>
inline T squared_mag(quaternion<T> const& q)
{
    return q.squared_mag();
}

// return the conjugate of the quaternion
template<typename T>
inline quaternion<T> conjugate(quaternion<T> const& q)
{
    return quaternion<T>(q.w(), -q.x(), -q.y(), -q.z());
}

template<typename T>
inline quaternion<T> invert(quaternion<T> const& q, T tolerance = math_trait<T>::zero_tolerance())
{
    quaternion<T> res(q);
    res.invert(tolerance);
    return res;
}

template<typename T>
inline bool invert(quaternion<T> const& q, quaternion<T>& res, T tolerance = math_trait<T>::zero_tolerance())
{
    res = q;
    return res.invert(tolerance);
}

template<typename T>
inline quaternion<T> operator *(quaternion<T> lhs, quaternion<T> const& rhs)
{
    return lhs *= rhs;
}

template<typename T>
inline quaternion<T> operator +(quaternion<T> lhs, quaternion<T> const& rhs)
{
    return lhs += rhs;
}

template<typename T>
inline quaternion<T> operator -(quaternion<T> lhs, quaternion<T> const& rhs)
{
    return lhs -= rhs;
}

template<typename T>
inline quaternion<T> operator *(T scale, quaternion<T> rhs)
{
    rhs *= scale;
    return rhs;
}

template<typename T>
inline quaternion<T> operator *(quaternion<T> lhs, T scale)
{
    lhs *= scale;
    return lhs;
}

// Rotate a vector by a given unit quaternion.
// The rotated vector is calculated as
//    v' = qvq*
template<typename T>
inline vector3<T> operator *(quaternion<T> const& q, vector3<T> const& v)
{
    KISMET_ASSERT(approx(squared_mag(q), T(1.0)));

    T vqv2 = T(2.0) * (q.x() * v.x() + q.y() * v.y() + q.z() * v.z());
    T vqvq = q.x() * q.x() + q.y() * q.y() + q.z() * q.z();
    T qww = q.w() * q.w();
    T qw2 = T(2.0) * q.w();

    vector3<T> res;
    res.x(vqv2 * q.x() + qww * v.x() + qw2 * (q.y() * v.z() - q.z() * v.y()) - v.x() * vqvq);
    res.y(vqv2 * q.y() + qww * v.y() + qw2 * (q.z() * v.x() - q.x() * v.z()) - v.y() * vqvq);
    res.z(vqv2 * q.z() + qww * v.z() + qw2 * (q.x() * v.y() - q.y() * v.x()) - v.z() * vqvq);
    return res;
}

// linear interpolate q0 and q1, return the normalized result
template<typename T>
quaternion<T> nlerp(quaternion<T> const& q0, quaternion<T> const& q1, T t)
{
    quaternion<T> res;
    res[0] = lerp(q0[0], q1[0], t);
    res[1] = lerp(q0[1], q1[1], t);
    res[2] = lerp(q0[2], q1[2], t);
    res[3] = lerp(q0[3], q1[3], t);
    res.normalize();
    return res;   
}

// return the dot product of two quaternion
template<typename T>
T dot(quaternion<T> const& q0, quaternion<T> const& q1)
{
    return q0.dot(q1);
}

// return the spherical linear interpolation of q0 and q1
template<typename T>
quaternion<T> slerp(quaternion<T> const& q0, quaternion<T> const& q1, T t)
{
    KISMET_ASSERT(t >= 0 && t <= 1);

    // calculate the angle between q0 and q1
    T c = dot(q0, q1);

    // if the angle is nearly 0, we should fallback to nlerp to avoid numeric issue
    if (std::abs(c) >= T(0.9999))
    {
        return nlerp(q0, q1, t);
    }

    T angle = std::acos(c);
    T sin0 = std::sin((1 - t) * angle);
    T sin1 = std::sin(t * angle);
    T denom = T(1.0) / std::sin(angle);

    // make sure slerp along the shortest path
    if (c < T(0))
    {
        return sin0 * denom * q0 - sin1 * denom * q1;
    }
    return sin0 * denom * q0 + sin1 * denom * q1;
}

template<typename T>
inline bool operator ==(quaternion<T> const& lhs, quaternion<T> const& rhs)
{
    return std::equal(lhs.data(), lhs.data() + 4, rhs.data(), approximate<T>());
}

template<typename T>
inline bool operator !=(quaternion<T> const& lhs, quaternion<T> const& rhs)
{
    return !(lhs == rhs);
}

template<typename T>
inline std::ostream& operator <<(std::ostream& os, quaternion<T> const& v)
{
    return os << "[ " << v.w() << ", " << v.x() << ", " << v.y() << ", " << v.z() << " ]";
}

using quaternionf = quaternion<float>;
using quaterniond = quaternion<double>;

} // namespace math
} // namespace kismet

#endif // KIMSET_MATH_QUATERNION_H
