#ifndef KISMET_MATH_BASIC_MATRIX_H
#define KISMET_MATH_BASIC_MATRIX_H

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <initializer_list>
#include <numeric>
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
struct matrix_size
{
    enum { value = N * matrix_size<Sizes...>::value };
};

template<std::size_t N>
struct matrix_size<N>
{
    enum { value = N };
};

// Utility for making a nesting initializer list
template<typename T, std::size_t N>
struct make_matrix_initializer
{
    using type = std::initializer_list<typename make_matrix_initializer<T, N - 1>::type>;
};

template<typename T>
struct make_matrix_initializer<T, 1>
{
    using type = std::initializer_list<T>;
};

template<typename T, std::size_t N>
using matrix_initializer = typename make_matrix_initializer<T, N>::type;

template<typename T, typename U, std::size_t N>
T* insert_flat(T* data, std::initializer_list<U> const& l)
{
    for (auto& e : l) {
        data = insert_flat(data, e);
    }
    return data;
}

template<typename T>
T* insert_flat(T* data, std::initializer_list<T> const& l)
{
    std::copy(l.begin(), l.end(), data);
    return data + l.size();
}

template<typename Derived, typename T, std::size_t... Sizes>
class common_matrix_impl
{
    static_assert(detail::valid_dimension<Sizes...>::value, "Dimension must not be 0");
public:
    using size_type              = std::size_t;
    using difference_type        = std::ptrdiff_t;
    using value_type             = T;
    using reference_type         = T&;
    using iterator               = T*;
    using const_iterator         = T const*;
    using reverse_iterator       = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    // number of dimension of the matrix
    static const size_type rank = sizeof...(Sizes);
    // number of elements
    static const size_type num  = detail::matrix_size<Sizes...>::value;

    // default constructor, do nothing
    common_matrix_impl() {}
    
    common_matrix_impl(matrix_initializer<T, rank> const& mi)
    {
        insert_flat(data(), mi);
    }

    // element-wise addition
    Derived& operator +=(Derived const& m)
    {
        for (size_type i = 0; i < num; ++i) {
            m_a[i] += m.m_a[i];
        }
        return static_cast<Derived&>(*this);
    }

    // element-wise subtraction
    Derived& operator -=(Derived const& m)
    {
        for (size_type i = 0; i < num; ++i) {
            m_a[i] -= m.m_a[i];
        }
        return static_cast<Derived&>(*this);
    }

    // scalar multiplication
    Derived& operator *=(T k)
    {
        for (auto& e : m_a) {
            e *= k;
        }
        return static_cast<Derived&>(*this);
    }

    // scalar division
    Derived& operator /=(T k)
    {
        assert(!is_zero(k));
        *this *= inv(k);
        return static_cast<Derived&>(*this);
    }

    // get the size of n-th dimension
    size_type size(size_t index) const
    {
        assert(index < rank);
        return get(index, integer_sequence<size_type, Sizes...>());
    }

    // get the total number of elements
    size_type size() const { return num; }

    T*       data() { return m_a;  }
    T const* data() const { return m_a; }

    iterator       begin() { return iterator{ m_a }; }
    iterator       end() { return iterator{ m_a + size() }; }
    const_iterator begin() const { return m_a; }
    const_iterator end() const { return m_a + size(); }

    const_iterator cbegin() const { return begin(); }
    const_iterator cend() const { return end();  }

    reverse_iterator rbegin() { return reverse_iterator{ end() }; }
    reverse_iterator rend() { return reverse_iterator{ begin() }; }

    const_reverse_iterator rbegin() const { return const_reverse_iterator{ end() }; }
    const_reverse_iterator rend() const { return const_reverse_iterator{ begin() }; }

    const_reverse_iterator crbegin() const { return rbegin(); }
    const_reverse_iterator crend() const { return rend(); }
protected:
    T m_a[num];
};

} // namespace detail

// fixed sized matrix with arbitrary number of dimsensions
template<typename T, std::size_t... Sizes>
class basic_matrix : public detail::common_matrix_impl<basic_matrix<T, Sizes...>, T, Sizes...>
{
    using base_type = detail::common_matrix_impl<basic_matrix<T, Sizes...>, T, Sizes...>;
public:
    basic_matrix() {}

    basic_matrix(detail::matrix_initializer<T, base_type::rank> const& mi)
        : base_type{ mi }
    {
    }
};

template<typename T, std::size_t... Sizes>
inline basic_matrix<T, Sizes...> operator +(basic_matrix<T, Sizes...> m1, basic_matrix<T, Sizes...> const& m2)
{
    return m1 += m2;
}

template<typename T, std::size_t... Sizes>
inline basic_matrix<T, Sizes...> operator -(basic_matrix<T, Sizes...> m1, basic_matrix<T, Sizes...> const& m2)
{
    return m1 -= m2;
}

template<typename T, std::size_t... Sizes>
inline basic_matrix<T, Sizes...> operator *(T k, basic_matrix<T, Sizes...> m)
{
    m *= k;
    return m;
}

template<typename T, std::size_t... Sizes>
inline basic_matrix<T, Sizes...> operator *(basic_matrix<T, Sizes...> m, T k)
{
    m *= k;
    return m;
}

template<typename T, std::size_t... Sizes>
inline basic_matrix<T, Sizes...> operator /(basic_matrix<T, Sizes...> m, T k)
{
    m /= k;
    return m;
}

template<typename T, std::size_t... Sizes>
inline bool operator ==(basic_matrix<T, Sizes...> const& m1, basic_matrix<T, Sizes...> const& m2)
{
    return std::equal(m1.begin(), m1.end(), m2.begin());
}

template<typename T, std::size_t... Sizes>
inline bool operator !=(basic_matrix<T, Sizes...> const& m1, basic_matrix<T, Sizes...> const& m2)
{
    return !(m1 == m2);
}

// avoid instantiating 0-d matrix
template<typename T>
class basic_matrix<T>;

template<typename T, std::size_t N>
inline T operator *(basic_matrix<T, N> const& m1, basic_matrix<T, N> const& m2)
{
    return std::inner_product(m1.begin(), m1.end(), m2.begin(), T{});
}

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
