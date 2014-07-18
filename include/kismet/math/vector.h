#ifndef KISMET_MATH_VECTOR_H
#define KISMET_MATH_VECTOR_H

#include <algorithm>
#include <cstddef>
#include <initializer_list>
#include <iterator>
#include <numeric>
#include <ostream>
#include <utility>
#include "kismet/common_type.h"
#include "kismet/core.h"
#include "kismet/enable_if_convertible.h"
#include "kismet/math/detail/vector_common.h"
#include "kismet/utility.h"

namespace kismet
{
namespace math
{

namespace detail
{

template<typename Derived, typename T, std::size_t N>
struct vector_base
{
    static_assert(N > 0, "N must be positive");

    using size_type              = std::size_t;
    using value_type             = T;
    using reference              = T&;
    using const_reference        = T const&;
    using pointer                = T*;
    using const_pointer          = T const*;
    using iterator               = T*;
    using const_iterator         = T const*;
    using reverse_iterator       = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    vector_base() = default;

    template<typename U>
    vector_base(std::initializer_list<U> list)
    {
        assign(list.begin(), list.end());
    }

    template<typename U>
    vector_base(vector_base<Derived, U, N> const& rhs)
        : v{ rhs.v }
    {
    }

    template<typename U>
    enable_if_convertible_t<U, T, Derived&> operator =(vector_base<Derived, U, N> const& rhs)
    {
        std::copy(rhs.v, rhs.v + N, v);
        return static_cast<Derived&>(*this);
    }

    template<typename U>
    enable_if_convertible_t<U, T, Derived&> operator =(std::initializer_list<U> list)
    {
        assign(list.begin(), list.end());
        return static_cast<Derived&>(*this);
    }

    // assign from a sequence, number of elements must be <= N
    // if size of sequence is less than N, remaining elements are zero filled
    template<typename InputIt>
    void assign(InputIt start, InputIt end)
    {
        checked_copy(start, end, N, v, T(0));
    }

    pointer data() { return v; }
    const_pointer data() const { return v; }

    size_type size() const { return N; }

    iterator begin() { return v; }
    iterator end() { return v + N; }

    const_iterator begin() const { return v; }
    const_iterator end() const { return v + N; }

    const_iterator cbegin() const { return v; }
    const_iterator cend() const { return v + N; }

    reverse_iterator rbegin() { return reverse_iterator{ end() }; }
    reverse_iterator rend() { return reverse_iterator{ begin() }; };

    const_reverse_iterator rbegin() const { return const_reverse_iterator{ end() }; }
    const_reverse_iterator rend() const { return const_reverse_iterator{ begin() }; }

    const_reverse_iterator crbegin() const { return rbegin(); }
    const_reverse_iterator crend() const { return rend(); }

    Derived& operator +=(Derived const& rhs)
    {
        for (std::size_t i = 0; i < N; ++i)
        {
            v[i] += rhs.v[i];
        }
        return static_cast<Derived&>(*this);
    }

    Derived& operator -=(Derived const& rhs)
    {
        for (std::size_t i = 0; i < N; ++i)
        {
            v[i] -= rhs.v[i];
        }
        return static_cast<Derived&>(*this);
    }

    Derived& operator *=(T k)
    {
        std::for_each(v, v + N, [k](T& a) { a *= k; });
        return static_cast<Derived&>(*this);
    }

    Derived& operator /=(T k)
    {
        KISMET_ASSERT(!is_zero(k));
        return *this *= inv(k);
    }

    bool normalize(T tolerance = math_trait<T>::zero_tolerance())
    {
        return detail::normalize(*this, tolerance);
    }

    reference operator [](size_type i)
    {
        KISMET_ASSERT(i < N);
        return v[i];
    }

    value_type operator [](size_type i) const
    {
        KISMET_ASSERT(i < N);
        return v[i];
    }

    // return the zero vector
    static Derived const zero;
protected:
    value_type v[N];
};

template<typename Derived, typename T, std::size_t N>
Derived const vector_base<Derived, T, N>::zero = { T(0) }; // non-empty initializer list, force filling zero.

} // namespace detail

template<typename T, std::size_t N>
class vector : public detail::vector_base<vector<T, N>, T, N>
{
    using base_type = detail::vector_base<vector<T, N>, T, N>;
public:
    vector() = default;

    template<typename U>
    vector(vector<U, N> const& rhs,
           enable_if_convertible_t<U, T>* = 0)
       : base_type{ rhs }
    {
    }

    template<typename U>
    vector(std::initializer_list<U> list, enable_if_convertible_t<U, T>* = 0)
        : base_type(list)
    {
    }
};

template<typename T>
class vector<T, 2> : public detail::vector_base<vector<T, 2>, T, 2>
{
    using base_type = detail::vector_base<vector<T, 2>, T, 2>;
public:
    vector() = default;

    vector(T x, T y)
    {
        this->x(x).y(y);
    }

    template<typename U>
    vector(vector<U, 2> const& rhs,
           enable_if_convertible_t<U, T>* = 0)
        : base_type{rhs}
    {
    }

    template<typename U>
    vector(std::initializer_list<U> list, enable_if_convertible_t<U, T>* = 0)
        : base_type(list)
    {
    }

    vector& x(T a) { this->v[0] = a; return *this; }
    vector& y(T a) { this->v[1] = a; return *this; }

    T x() const { return this->v[0]; }
    T y() const { return this->v[1]; }

    static vector const left;
    static vector const right;
    static vector const up;
    static vector const down;
};

template<typename T>
vector<T, 2> const vector<T, 2>::left(T(-1), T(0));

template<typename T>
vector<T, 2> const vector<T, 2>::right(T(1), T(0));

template<typename T>
vector<T, 2> const vector<T, 2>::up(T(0), T(1));

template<typename T>
vector<T, 2> const vector<T, 2>::down(T(0), T(-1));

template<typename T>
class vector<T, 3> : public detail::vector_base<vector<T, 3>, T, 3>
{
    using base_type = detail::vector_base<vector<T, 3>, T, 3>;
public:
    vector() = default;

    vector(T x, T y, T z)
    {
        this->x(x).y(y).z(z);
    }

    template<typename U>
    vector(vector<U, 3> const& rhs,
           enable_if_convertible_t<U, T>* = 0)
        : base_type{rhs}
    {
    }

    template<typename U>
    vector(std::initializer_list<U> list, enable_if_convertible_t<U, T>* = 0)
        : base_type(list)
    {
    }

    vector& x(T a) { this->v[0] = a; return *this; }
    vector& y(T a) { this->v[1] = a; return *this; }
    vector& z(T a) { this->v[2] = a; return *this; }

    T x() const { return this->v[0]; }
    T y() const { return this->v[1]; }
    T z() const { return this->v[2]; }

    static vector const left; 
    static vector const right;
    static vector const up;
    static vector const down;
    static vector const forward;
    static vector const back;
};

template<typename T>
vector<T, 3> const vector<T, 3>::left(T(-1), T(0), T(0));

template<typename T>
vector<T, 3> const vector<T, 3>::right(T(1), T(0), T(0));

template<typename T>
vector<T, 3> const vector<T, 3>::up(T(0), T(1), T(0));

template<typename T>
vector<T, 3> const vector<T, 3>::down(T(0), T(-1), T(0));

template<typename T>
vector<T, 3> const vector<T, 3>::forward(T(0), T(0), T(1));

template<typename T>
vector<T, 3> const vector<T, 3>::back(T(0), T(0), T(-1));

template<typename T>
class vector<T, 4> : public detail::vector_base<vector<T, 4>, T, 4>
{
    using base_type = detail::vector_base<vector<T, 4>, T, 4>;
public:
    vector() = default;

    vector(T x, T y, T z, T w)
    {
        this->x(x).y(y).z(z).w(w);
    }

    template<typename U>
    vector(vector<U, 4> const& rhs,
           enable_if_convertible_t<U, T>* = 0)
        : base_type{rhs}
    {
    }

    template<typename U>
    vector(std::initializer_list<U> list, enable_if_convertible_t<U, T>* = 0)
        : base_type(list)
    {
    }

    vector& x(T a) { this->v[0] = a; return *this; }
    vector& y(T a) { this->v[1] = a; return *this; }
    vector& z(T a) { this->v[2] = a; return *this; }
    vector& w(T a) { this->v[3] = a; return *this; }

    T x() const { return this->v[0]; }
    T y() const { return this->v[1]; }
    T z() const { return this->v[2]; }
    T w() const { return this->v[3]; }
};

// Return the dot product of two vectors
template<typename T, std::size_t N>
inline T dot(vector<T, N> const& a, vector<T, N> const& b)
{
    return detail::dot(a, b);
}

namespace detail
{

template<typename T>
inline void cross(T const* a, T const* b, T* c)
{
    // mnemonics xyzzy, see http://en.wikipedia.org/wiki/Cross_product 
    c[0] = a[1] * b[2] - a[2] * b[1];
    c[1] = a[2] * b[0] - a[0] * b[2];
    c[2] = a[0] * b[1] - a[1] * b[0];
}

}

// Return the cross product of two 3-d vectors
template<typename T>
inline vector<T, 3> cross(vector<T, 3> const& a, vector<T, 3> const& b)
{
    vector<T, 3> res;
    detail::cross(a.data(), b.data(), res.data());
    return res;
}

// Return the cross product of two augmented 3-d vectors, 
// the w components of two vectors must be 0
// 
// NOTE: Cross product is only defined in 3d/7d spaces, this is
// only meant to be used as convenience function for calculating
// cross product of two augmented 3d vectors whose w components
// are 0.
template<typename T>
inline vector<T, 4> cross(vector<T, 4> const& a, vector<T, 4> const& b)
{
    KISMET_ASSERT(a.w() == T(0) && b.w() == T(0));
    vector<T, 4> res;
    detail::cross(a.data(), b.data(), res.data());
    res[3] = T(0);
    return res;
}

template<typename T, std::size_t N>
inline vector<T, N> normalize(vector<T, N> v, T tolerance = math_trait<T>::zero_tolerance())
{
    v.normalize(tolerance);
    return v;
}

// Return true if normalization succeeds, 
// otherwise return false in which case whether res is changed is unspecified.
template<typename T, std::size_t N>
inline bool normalize(vector<T, N> const& v, 
                      vector<T, N>& res, 
                      T tolerance = math_trait<T>::zero_tolerance())
{
    res = v;
    return res.normalize(tolerance);
}

template<typename T, std::size_t N>
inline T squared_mag(vector<T, N> const& v)
{
    return detail::squared_mag(v);
}

template<typename T, std::size_t N>
inline T mag(vector<T, N> const& v)
{
    return detail::mag(v);
}

template<typename T, std::size_t N>
inline vector<T, N> operator +(vector<T, N> a, vector<T, N> const& b)
{
    a += b;
    return a;
}

#ifndef KISMET_MSC
template<typename T, typename U, std::size_t N>
inline vector<common_type_t<T, U>, N> operator +(vector<T, N> const& a, vector<U, N> const& b)
{
    vector<common_type_t<T, U>, N> res{ a };
    res += b;
    return res;
}
#endif

template<typename T, std::size_t N>
inline vector<T, N> operator -(vector<T, N> a, vector<T, N> const& b)
{
    a -= b;
    return a;
}

#ifndef KISMET_MSC
template<typename T, typename U, std::size_t N>
inline vector<common_type_t<T, U>, N> operator -(vector<T, N> const& a, vector<U, N> const& b)
{
    vector<common_type_t<T, U>, N> res{ a };
    res -= b;
    return res;
}
#endif

template<typename T, std::size_t N>
inline vector<T, N> operator *(vector<T, N> a, T k)
{
    a *= k;
    return a;
}

template<typename T, std::size_t N>
inline vector<T, N> operator *(T k, vector<T, N> a)
{
    a *= k;
    return a;
}

template<typename T, std::size_t N>
inline vector<T, N> operator /(vector<T, N> a, T k)
{
    a /= k;
    return a;
}

#ifndef KISMET_MSC
template<typename T, std::size_t N, typename U>
inline vector<common_type_t<T, U>, N> operator *(vector<T, N> const& a, U k)
{
    vector<common_type_t<T, U>, N> v { a };
    v *= k;
    return v;
}

template<typename U, typename T, std::size_t N>
inline vector<common_type_t<T, U>, N> operator *(U k, vector<T, N> const& a)
{
    vector<common_type_t<T, U>, N> v { a };
    v *= k;
    return v;
}

template<typename T, std::size_t N, typename U>
inline vector<common_type_t<T, U>, N> operator /(vector<T, N> const& a, U k)
{
    vector<common_type_t<T, U>, N> v { a };
    v /= k;
    return v;
}
#endif

/// Left multiply a vector with a matrix
/// Returns v * M
template<typename T, std::size_t N1, std::size_t N2>
inline vector<T, N1> operator *(vector<T, N1> const& v, matrix<T, N1, N2> const& m)
{
    vector<T, N1> u;
    for (std::size_t i = 0; i < N1; ++i)
    {
        u[i] = T(0);
        for (std::size_t j = 0; j < N2; ++j)
        {
            u[i] += v[j] * m[j][i];
        }
    }
    return u;
}

/// Right multiply a vector with a matrix
/// Returns M * v
template<typename T, std::size_t N1, std::size_t N2>
inline vector<T, N2> operator *(matrix<T, N1, N2> const& m, vector<T, N2> const& v)
{
    vector<T, N2> u;
    for (std::size_t i = 0; i < N1; ++i)
    {
        u[i] = T(0);
        for (std::size_t j = 0; j < N2; ++j)
        {
            u[i] += m[i][j] * v[j];
        }
    }
    return u;
}

template<typename T, typename U, std::size_t N>
inline bool operator ==(vector<T, N> const& a, vector<U, N> const& b)
{
    return std::equal(a.begin(), a.end(), b.begin());
}

template<typename T, typename U, std::size_t N>
inline bool operator !=(vector<T, N> const& a, vector<U, N> const& b)
{
    return !(a == b);
}

template<typename T, std::size_t N>
inline std::ostream& operator <<(std::ostream& os, vector<T, N> const& v)
{
    os << "{ "
        << v[0];
    for (std::size_t i = 1; i < N; ++i)
    {
        os << ", " << v[i];
    }
    os << " }";
    return os;
}

using vector2f = vector<float, 2>;
using vector3f = vector<float, 3>;
using vector4f = vector<float, 4>;

using vector2d = vector<double, 2>;
using vector3d = vector<double, 3>;
using vector4d = vector<double, 4>;

} // namespace math
} // namespace kismet

#endif // KISMET_MATH_VECTOR_H

