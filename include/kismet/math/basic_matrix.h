#ifndef KISMET_MATH_BASIC_MATRIX_H
#define KISMET_MATH_BASIC_MATRIX_H

#include <cassert>
#include <cstddef>
#include <algorithm>
#include <iterator>
#include "kismet/math/math_trait.h"
#include "kismet/integer_sequence.h"
#include "kismet/utility.h"

namespace kismet
{
namespace math
{

namespace detail
{

// trait used to check if any given dimension is 0
template<std::size_t... Sizes>
struct valid_dimension
{
    enum { value = true };
};

template<std::size_t N, std::size_t... Sizes>
struct valid_dimension<N, Sizes...>
{
    enum { value = N != 0 && valid_dimension<Sizes...>::value };
};

template<std::size_t N, std::size_t... Sizes>
struct matrix_dim
{
    enum { value = N * matrix_dim<Sizes...>::value };
};

template<std::size_t N>
struct matrix_dim<N>
{
    enum { value = N };
};

template<typename T, typename Derived, std::size_t... Sizes>
class common_matrix_impl
{
    static_assert(detail::valid_dimension<Sizes...>::value, "Dimension must not be 0");
public:
    // element-wise addition
    Derived& operator +=(Derived const& m);
    // element-wise subtraction
    Derived& operator -=(Derived const& m);
    // scalar multiplication
    Derived& operator *=(T k);
    // scalar division
    Derived& operator /=(T k);

    std::size_t dim(size_t index) const;
protected:
    T m_a[detail::matrix_dim<Sizes...>::value];
};

template<typename T, typename Derived, std::size_t... Sizes>
inline Derived& common_matrix_impl<T, Derived, Sizes...>::operator +=(Derived const& m)
{
    for (std::size_t i = 0; i < sizeof(m_a) / sizeof(m_a[0]); ++i) {
        m_a[i] += m.m_a[i];
    }
    return static_cast<Derived&>(*this);
}

template<typename T, typename Derived, std::size_t... Sizes>
inline Derived& common_matrix_impl<T, Derived, Sizes...>::operator -=(Derived const& m)
{
    for (std::size_t i = 0; i < sizeof(m_a) / sizeof(m_a[0]); ++i) {
        m_a[i] -= m.m_a[i];
    }
    return static_cast<Derived&>(*this);
}

template<typename T, typename Derived, std::size_t... Sizes>
inline Derived& common_matrix_impl<T, Derived, Sizes...>::operator *=(T k)
{
    for (auto& e : m_a) {
        e *= k;
    }
    return static_cast<Derived&>(*this);
}

template<typename T, typename Derived, std::size_t... Sizes>
inline Derived& common_matrix_impl<T, Derived, Sizes...>::operator /=(T k)
{
    using std::abs;

    assert(abs(k) > math_trait<T>::zero_tolerance());
    *this *= inv(k);
    return static_cast<Derived&>(*this);
}

template<typename T, typename Derived, std::size_t... Sizes>
inline std::size_t common_matrix_impl<T, Derived, Sizes...>::dim(std::size_t index) const
{
    assert(index < sizeof...(Sizes));
    return get(index, utility::integer_sequence<std::size_t, Sizes...>());
}
} // namespace detail

// fixed sized matrix with arbitrary number of dimsensions
template<typename T, std::size_t... Sizes>
class basic_matrix : public detail::common_matrix_impl<T, basic_matrix<T, Sizes...>, Sizes...>
{
};

template<typename T, std::size_t... Sizes>
inline basic_matrix<T, Sizes...> operator *(T k, basic_matrix<T, Sizes...> const& m)
{
    basic_matrix<T, Sizes...> tmp{ m };
    tmp *= k;
    return tmp;
}

// avoid instantiating 0-d matrix
template<typename T>
class basic_matrix<T>;

// 2-d matrix
template<typename T, std::size_t N1, std::size_t N2>
class basic_matrix<T, N1, N2>
{
    static_assert(detail::valid_dimension<N1, N2>::value, "Dimension must not be 0");
private:
    T m_a[N1][N2];
};

KISMET_CLASS_TEMPLATE_API(basic_matrix, float, 2)
KISMET_CLASS_TEMPLATE_API(basic_matrix, float, 3)
KISMET_CLASS_TEMPLATE_API(basic_matrix, float, 4)

KISMET_CLASS_TEMPLATE_API(basic_matrix, double, 2)
KISMET_CLASS_TEMPLATE_API(basic_matrix, double, 3)
KISMET_CLASS_TEMPLATE_API(basic_matrix, double, 4)

KISMET_CLASS_TEMPLATE_API(basic_matrix, float, 2, 2)
KISMET_CLASS_TEMPLATE_API(basic_matrix, float, 3, 3)
KISMET_CLASS_TEMPLATE_API(basic_matrix, float, 4, 4)

KISMET_CLASS_TEMPLATE_API(basic_matrix, double, 2, 2)
KISMET_CLASS_TEMPLATE_API(basic_matrix, double, 3, 3)
KISMET_CLASS_TEMPLATE_API(basic_matrix, double, 4, 4)

} // namespace math
} // namespace kismet

#endif // KISMET_MATH_BASIC_MATRIX_H
