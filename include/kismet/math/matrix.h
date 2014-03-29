#ifndef KISMET_MATH_MATRIX_H
#define KISMET_MATH_MATRIX_H

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <initializer_list>
#include <numeric>
#include <ostream>
#include <type_traits>
#include <utility>

#include "kismet/core/assert.h"
#include "kismet/math/math_trait.h"
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

template<typename T>
inline T* insert_flat(T* data, std::initializer_list<T> const& l)
{
    std::copy(l.begin(), l.end(), data);
    return data + l.size();
}

template<typename T, typename U>
inline T* insert_flat(T* data, std::initializer_list<U> const& l)
{
    for (auto& e : l) {
        data = insert_flat(data, e);
    }
    return data;
}

template<std::size_t S1, std::size_t S2>
struct is_row : std::false_type {};

template<>
struct is_row<1, 1> : std::true_type {};

} // namespace detail

// Represents a row or column of a given matrix.
// The reference must not outlive the referneced matrix.
// N is the size of the row or the column
// S is the stride between elements.
template<typename T, std::size_t N, std::size_t S>
class matrix_vector_ref
{
public:
    enum { rank = 1, num = N };

    using size_type       = std::size_t;
    using difference_type = std::ptrdiff_t;
    using value_type      = typename std::remove_const<T>::type;
    using reference       = T&;
    using pointer         = T*;

    matrix_vector_ref(pointer p)
        : m_p(p)
    {
    }

    // convert a row/column type to const row/column type
    template<typename U>
    matrix_vector_ref(matrix_vector_ref<U, N, S> const& v,
                      typename std::enable_if<
                            !std::is_const<U>::value &&
                            std::is_same<value_type, U>::value>::type* = 0)
        : m_p(v.m_p)
    {
    }

    pointer data() { return m_p; }
    pointer data() const { return m_p; }

    size_type size() const { return N; }

    reference operator [](size_type index)
    {
        KISMET_ASSERT(index < N);
        return m_p[index * S];
    }

    reference const& operator [](size_type index) const
    {
        KISMET_ASSERT(index < N);
        return m_p[index * S];
    }

    // assign from a row/column
    template<typename U, std::size_t S2>
    typename std::enable_if<
                !std::is_const<T>::value
                    && std::is_same<
                            typename std::remove_const<U>::type
                          , value_type
                       >::value
              , matrix_vector_ref>::type&
    assign(matrix_vector_ref<U, N, S2> v)
    {
        if (data() != v.data())
        {
            copy(v, detail::is_row<S, S2>());
        }
        return *this;
    }

    // assign from an array with size N
    matrix_vector_ref& assign(value_type const* p)
    {
        KISMET_ASSERT(p);
        assign(matrix_vector_ref<value_type const, N, S>{p});
        return *this;
    }

    // compare a row/column to a row/column
    template<typename U, std::size_t S2>
    typename std::enable_if<
                std::is_same<
                    typename std::remove_const<U>::type
                  , value_type
                >::value,
                bool>::type
    operator ==(matrix_vector_ref<U, N, S2> v) const
    {
        return data() == v.data() || equal(v, detail::is_row<S, S2>());
    }

private:
    // copy row to row
    template<typename U>
    void copy(matrix_vector_ref<U, N, S> v, std::true_type)
    {
        std::copy(v.data(), v.data() + N, data());
    }

    // copy row to column, or column to column, or column to row
    template<typename U, std::size_t S2>
    void copy(matrix_vector_ref<U, N, S2> v, std::false_type)
    {
        for (size_type i = 0; i < N; ++i)
        {
            (*this)[i] = v[i];
        }
    }

    // compare row to row
    template<typename U>
    bool equal(matrix_vector_ref<U, N, S> v, std::true_type) const
    {
        return std::equal(data(), data() + N, v.data());
    }

    // compare row to column, or column to column, or column to row
    template<typename U, std::size_t S2>
    bool equal(matrix_vector_ref<U, N, S2> v, std::false_type) const
    {
        for (size_type i = 0; i < N; ++i)
        {
            if ((*this)[i] != v[i])
            {
                return false;
            }
        }
        return true;
    }

    T* m_p;
};

template<typename T, std::size_t N1, std::size_t N2>
std::ostream& operator <<(std::ostream& os, matrix_vector_ref<T, N1, N2> const& v)
{
    os << "{ ";
    for (std::size_t i = 0; i < v.size(); ++i)
    {
        if (i != 0)
        {
            os << ", ";
        }
        os << v[i];
    }
    os << " }";
    return os;
}

// 2d dimensional array
template<typename T, std::size_t N1, std::size_t N2>
class matrix
{
    static_assert(detail::valid_dimension<N1, N2>::value, "Dimension must not be 0");
public:
    using size_type       = std::size_t;
    using difference_type = std::ptrdiff_t;
    using value_type      = T;
    using reference_type  = T&;
    using iterator        = T*;
    using const_iterator  = T const*;

    using row_type        = matrix_vector_ref<T, N1, 1>;
    using const_row_type  = matrix_vector_ref<T const, N1, 1>;
    using col_type        = matrix_vector_ref<T, N1, N2>;
    using const_col_type  = matrix_vector_ref<T const, N1, N2>;

    enum { rank = 2, num = N1 * N2 };

    matrix() {}
    
    matrix(detail::matrix_initializer<T, rank> const& mi)
    {
        detail::insert_flat(data(), mi);
    }

    matrix(T const* p)
    {
        std::copy(p, p + num, &m_a[0][0]);
    }

    // element-wise addition
    matrix& operator +=(matrix const& m)
    {
        for (size_type i = 0; i < N1; ++i)
        {
            for (size_type j = 0; j < N2; ++j)
            {
                m_a[i][j] += m.m_a[i][j];
            }
        }
        return *this;
    }

    // element-wise subtraction
    matrix& operator -=(matrix const& m)
    {
        for (size_type i = 0; i < N1; ++i)
        {
            for (size_type j = 0; j < N2; ++j)
            {
                m_a[i][j] -= m.m_a[i][j];
            }
        }
        return *this;
    }

    // scalar multiplication
    matrix& operator *=(T k)
    {
        for (size_type i = 0; i < N1; ++i)
        {
            for (size_type j = 0; j < N2; ++j)
            {
                m_a[i][j] *= k;
            }
        }
        return *this;
    }

    // scalar division
    matrix& operator /=(T k)
    {
        assert(!is_zero(k));
        return *this *= inv(k);
    }

    // Return the row object
    row_type row(size_type index)
    {
        KISMET_ASSERT(index < N1);
        return { m_a[index] };
    }

    row_type operator [](size_type index)
    {
        return row(index);
    }

    const_row_type operator [](size_type index) const
    {
        return row(index);
    }

    // Return the row object
    const_row_type row(size_type index) const
    {
        KISMET_ASSERT(index < N1);
        return { m_a[index] };
    }

    // Return the column object
    col_type col(size_type index)
    {
        KISMET_ASSERT(index < N2);
        return { &m_a[0][index] };
    }

    // Return the column object
    const_col_type col(size_type index) const
    {
        KISMET_ASSERT(index < N2);
        return { &m_a[0][index] };
    }

    // Return the dimension of a rank
    size_type extent(size_type index) const
    {
        KISMET_ASSERT(index < rank);
        return index == 0 ? N1 : N2;
    }

    // get the total number of elements
    size_type size() const { return num; }

    T*       data() { return &m_a[0][0];  }
    T const* data() const { return &m_a[0][0]; }

    iterator       begin() { return { data() }; }
    iterator       end() { return { data() + size() }; }

    const_iterator begin() const { return data(); }
    const_iterator end() const { return data() + size(); }

    const_iterator cbegin() const { return begin(); }
    const_iterator cend() const { return end();  }
private:
    T m_a[N1][N2];
};

template<typename T, std::size_t N1, std::size_t N2>
inline matrix<T, N1, N2> operator +(matrix<T, N1, N2> m1, matrix<T, N1, N2> const& m2)
{
    return m1 += m2;
}

template<typename T, std::size_t N1, std::size_t N2>
inline matrix<T, N1, N2> operator -(matrix<T, N1, N2> m1, matrix<T, N1, N2> const& m2)
{
    return m1 -= m2;
}

template<typename U, typename T, std::size_t N1, std::size_t N2>
inline matrix<T, N1, N2> operator *(U k, matrix<T, N1, N2> m)
{
    m *= k;
    return m;
}

template<typename U, typename T, std::size_t N1, std::size_t N2>
inline matrix<T, N1, N2> operator *(matrix<T, N1, N2> m, U k)
{
    m *= k;
    return m;
}

template<typename T, std::size_t N1, std::size_t N2>
inline matrix<T, N1, N2> operator /(matrix<T, N1, N2> m, T k)
{
    m /= k;
    return m;
}

template<typename T, std::size_t N1, std::size_t N2>
inline bool operator ==(matrix<T, N1, N2> const& m1, matrix<T, N1, N2> const& m2)
{
    return std::equal(m1.begin(), m1.end(), m2.begin());
}

template<typename T, std::size_t N1, std::size_t N2>
inline bool operator !=(matrix<T, N1, N2> const& m1, matrix<T, N1, N2> const& m2)
{
    return !(m1 == m2);
}

template<typename T, std::size_t N1, std::size_t N2>
std::ostream& operator <<(std::ostream& os, matrix<T, N1, N2> const& m)
{
    os << "{\n";
    for (std::size_t i = 0; i < N1; ++i)
    {
        os << " { ";
        for (std::size_t j = 0; j < N2; ++j)
        {
            if (j != 0)
            {
                os << ", ";
            }
            os << m[i][j];
        }
        os << " }\n";
    }
    os << "}";
    return os;
}

using matrix22f = matrix<float, 2, 2>;
using matrix33f = matrix<float, 3, 3>;
using matrix44f = matrix<float, 4, 4>;

using matrix22d = matrix<double, 2, 2>;
using matrix33d = matrix<double, 3, 3>;
using matrix44d = matrix<double, 4, 4>;

} // namespace math
} // namespace kismet

#endif // KISMET_MATH_matrix_H
