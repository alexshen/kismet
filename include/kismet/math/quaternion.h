#ifndef KIMSET_MATH_QUATERNION_H
#define KIMSET_MATH_QUATERNION_H

#include "kismet/math/math_trait.h"
#include "kismet/math/vector.h"
#include <ostream>
#include <algorithm>
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
        KISMET_ASSERT(approx(mag(v), T(1.0)));

        using std::cos;
        using std::sin;

        T ha = angle * T(0.5);
        T sin_ha = sin(ha);

        set(cos(ha), v.x() * sin_ha, v.y() * sin_ha, v.z() * sin_ha);
    }

    // normalize the quaternion
    bool normalize(T tolerance = math_trait<T>::zero_tolerance())
    {
        T len = mag(*this);
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
        T sm = squared_mag(*this);
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

template<typename T>
inline T squared_mag(quaternion<T> const& q)
{
    return q[0] * q[0] + q[1] * q[1] + q[2] * q[2] + q[3] * q[3];
}

// return the magnitude of the quaternion
template<typename T>
inline T mag(quaternion<T> const& q)
{
    using std::sqrt;
    return sqrt(squared_mag(q));
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
inline quaternion<T> operator *(T scale, quaternion<T> const& rhs)
{
    quaternion<T> res(rhs);
    res *= scale;
    return res;
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