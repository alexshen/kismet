#ifndef KISMET_MATH_MATRIX_H
#define KISMET_MATH_MATRIX_H

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <initializer_list>
#include <iterator>
#include <numeric>
#include <ostream>
#include <type_traits>
#include <utility>

#include "kismet/core/assert.h"
#include "kismet/math/math_trait.h"
#include "kismet/enable_if_convertible.h"
#include "kismet/is_either_convertible.h"
#include "kismet/strided_iterator.h"

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
using all_row_vectors = std::integral_constant<bool, S1 == S2 && S1 == 1>;

template<typename T, std::size_t N, std::size_t S>
class matrix_vector_ref_base
{
    template<typename U, std::size_t N1, std::size_t S1>
    friend class matrix_vector_ref_base;
public:
    using size_type              = std::size_t;
    using reference              = T&;
    using const_reference        = T const&;
    using pointer                = T*;
    using const_pointer          = T const*;
    using iterator               = strided_iterator<pointer>;
    using const_iterator         = strided_iterator<const_pointer>;

    template<typename U>
    matrix_vector_ref_base(U* start, U* p, U* end)
        : m_start(start)
        , m_p(p)
        , m_end(end)
    {
        KISMET_ASSERT(start && p && end);
    }

    template<typename U>
    matrix_vector_ref_base(matrix_vector_ref_base<U, N, S> const& v)
        : matrix_vector_ref_base(v.m_start, v.m_p, v.m_end)
    {
    }

    reference operator [](size_type index)
    {
        KISMET_ASSERT(index < N);
        return m_p[index * S];
    }

    const_reference operator [](size_type index) const
    {
        KISMET_ASSERT(index < N);
        return m_p[index * S];
    }

    iterator begin() { return {m_start, m_p, m_end, S}; }
    iterator end()   { return {m_start, m_p, m_end, S}; }

    const_iterator begin() const { return {m_start, m_p, m_end, S}; }
    const_iterator end()   const { return {m_start, m_p, m_end, S}; }
protected:
    pointer m_start; // matrix start
    pointer m_p;
    pointer m_end;   // matrix end
};

// optimization for row vector, no need to store matrix start and end
// as the iterator is not strided.
template<typename T, std::size_t N>
class matrix_vector_ref_base<T, N, 1>
{
    template<typename U, std::size_t N1, std::size_t S1>
    friend class matrix_vector_ref_base;
public:
    using size_type              = std::size_t;
    using reference              = T&;
    using const_reference        = T const&;
    using pointer                = T*;
    using const_pointer          = T const*;
    using iterator               = pointer;
    using const_iterator         = const_pointer;

    template<typename U>
    explicit matrix_vector_ref_base(U* p)
        : m_p(p)
    {
        KISMET_ASSERT(p);
    }

    template<typename U>
    matrix_vector_ref_base(U* start, U* p, U* end)
        : m_p(p)
    {
        KISMET_ASSERT(p);
    }

    template<typename U>
    matrix_vector_ref_base(matrix_vector_ref_base<U, N, 1> const& v)
        : m_p(v.m_p)
    {
    }

    reference operator [](size_type index)
    {
        KISMET_ASSERT(index < N);
        return m_p[index];
    }

    const_reference operator [](size_type index) const
    {
        KISMET_ASSERT(index < N);
        return m_p[index];
    }

    // only row type has data() as column type storage is not contiguous
    pointer       data()       { return this->m_p; }
    const_pointer data() const { return this->m_p; }

    iterator begin() { return m_p; }
    iterator end()   { return m_p + N; }

    const_iterator begin() const { return m_p; }
    const_iterator end()   const { return m_p + N; }
protected:
    pointer m_p;
};

} // namespace detail

// Represents a row or column of a given matrix.
// The reference must not outlive the referneced matrix.
// N is the size of the row or the column
// S is the stride between elements.
template<typename T, std::size_t N, std::size_t S>
class matrix_vector_ref : public detail::matrix_vector_ref_base<T, N, S>
{
    static_assert(S != 0, "stride cannot be 0");

    using base_type = detail::matrix_vector_ref_base<T, N, S>;
public:

    enum { rank = 1, num = N, stride = S };

    using difference_type        = std::ptrdiff_t;
    using value_type             = typename std::remove_const<T>::type;
    using pointer                = typename base_type::pointer;
    using const_pointer          = typename base_type::const_pointer;
    using size_type              = typename base_type::size_type;
    using const_iterator         = typename base_type::const_iterator;
    using reverse_iterator       = std::reverse_iterator<typename base_type::iterator>;
    using const_reverse_iterator = std::reverse_iterator<typename base_type::const_iterator>;

    template<typename U>
    explicit matrix_vector_ref(U* p,
                      typename std::enable_if<
                            S == 1 &&
                            std::is_convertible<U*, pointer>::value
                        >::type* = 0)
        : base_type(p)
    {
    }

    template<typename U>
    matrix_vector_ref(U* start, U* p, U* end, enable_if_convertible<U*, pointer>* = 0)
        : base_type(start, p, end)
    {
    }

    template<typename U>
    matrix_vector_ref(matrix_vector_ref<U, N, S> const& v,
                      enable_if_convertible_t<
                        typename matrix_vector_ref<U, N, S>::pointer, pointer
                      >* = 0)
        : base_type(v)
    {
    }

    size_type size() const { return N; }

    const_iterator cbegin() const { return this->begin(); }
    const_iterator cend()   const { return this->end(); }

    reverse_iterator rbegin() { return this->end(); }
    reverse_iterator rend()   { return this->begin(); }

    const_reverse_iterator rbegin() const { return rbegin(); }
    const_reverse_iterator rend() const   { return rend(); }

    const_reverse_iterator rcbegin() const { return rbegin(); }
    const_reverse_iterator rcend() const { return rend(); }

    matrix_vector_ref& operator =(matrix_vector_ref& v)
    {
        static_assert(!std::is_const<T>::value, "Cannot assign to non-const vector ref");
        copy_row_row(v, detail::all_row_vectors<S, S>());
        return *this;
    }

    // assign from a row/column
    template<typename U, std::size_t S2>
    typename std::enable_if<
        std::is_convertible<
            typename matrix_vector_ref<U, N, S2>::value_type,
            value_type>::value
        && !std::is_const<T>::value,
        matrix_vector_ref>::type&
    operator =(matrix_vector_ref<U, N, S2> const& v)
    {
        copy_row_row(v, detail::all_row_vectors<S, S2>());
        return *this;
    }

    // assign from a sequence
    template<typename Iter>
    typename std::enable_if<
        std::is_convertible<
            typename std::iterator_traits<Iter>::value_type,
            value_type>::value
        && !std::is_const<T>::value,
        matrix_vector_ref>::type&
    assign(Iter it)
    {
        using pointer_type = typename std::iterator_traits<Iter>::pointer;
        using pointer_row_vector
            = std::integral_constant<
                bool,
                S == 1 &&
                std::is_pointer<pointer_type>::value
              >;
        copy_pointer_row(it, pointer_row_vector());
        return *this;
    }

    // compare a row/column to a row/column
    template<typename U, std::size_t S2>
    typename std::enable_if<
        is_either_convertible<
            typename matrix_vector_ref<U, N, S2>::value_type,
            value_type>::value,
        bool>::type
    operator ==(matrix_vector_ref<U, N, S2> const& v) const
    {
        return equal_row_row(v, detail::all_row_vectors<S, S2>());
    }

private:
    template<typename It>
    void copy_pointer_row(It p, std::true_type)
    {
        std::copy(this->data(), this->data() + N, p);
    }

    // copy from a generic iterator to column/row
    template<typename It>
    void copy_pointer_row(It p, std::false_type)
    {
        for (size_type i = 0; i < N; ++i)
        {
            (*this)[i] = *p++;
        }
    }

    template<typename U>
    void copy_row_row(matrix_vector_ref<U, N, S> const& v, std::true_type)
    {
        std::copy(v.data(), v.data() + N, this->data());
    }

    // copy row to column, or column to column, or column to row
    template<typename U, std::size_t S2>
    void copy_row_row(matrix_vector_ref<U, N, S2> const& v, std::false_type)
    {
        for (size_type i = 0; i < N; ++i)
        {
            (*this)[i] = v[i];
        }
    }

    // compare row to row
    template<typename U>
    bool equal_row_row(matrix_vector_ref<U, N, S> const& v, std::true_type) const
    {
        return std::equal(this->data(), this->data() + N, v.data());
    }

    // compare row to column, or column to column, or column to row
    template<typename U, std::size_t S2>
    bool equal_row_row(matrix_vector_ref<U, N, S2> const& v, std::false_type) const
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

    pointer m_p;
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

// 2d dimensional matrix N1xN2
template<typename T, std::size_t N1, std::size_t N2>
class matrix
{
    static_assert(detail::valid_dimension<N1, N2>::value, "Dimension must not be 0");
public:
    using size_type       = std::size_t;
    using difference_type = std::ptrdiff_t;
    using value_type      = T;
    using reference       = T&;
    using const_reference = T const&;
    using pointer         = T*;
    using const_pointer   = T const*;
    using iterator        = T*;
    using const_iterator  = T const*;

    using row_type        = matrix_vector_ref<T, N2, 1>;
    using const_row_type  = matrix_vector_ref<T const, N2, 1>;
    using col_type        = matrix_vector_ref<T, N1, N2>;
    using const_col_type  = matrix_vector_ref<T const, N1, N2>;

    enum { rank = 2, num = N1 * N2 };

    matrix() {}
    
    matrix(detail::matrix_initializer<T, rank> const& mi)
    {
        detail::insert_flat(data(), mi);
    }

    // initialize from an array
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

    // Return the given row
    row_type operator [](size_type index)
    {
        return row(index);
    }

    const_row_type operator [](size_type index) const
    {
        return row(index);
    }

    // Return the row object
    row_type row(size_type index)
    {
        KISMET_ASSERT(index < N1);
        return row_type{ m_a[index] };
    }

    // Return the row object
    const_row_type row(size_type index) const
    {
        KISMET_ASSERT(index < N1);
        return const_row_type{ m_a[index] };
    }

    // Return the column object
    col_type col(size_type index)
    {
        KISMET_ASSERT(index < N2);
        return col_type{ data(), &m_a[0][index], data() + num };
    }

    // Return the column object
    const_col_type col(size_type index) const
    {
        KISMET_ASSERT(index < N2);
        return const_col_type{ data(), &m_a[0][index], data() + num };
    }

    // Return the size of a dimension
    size_type extent(size_type index) const
    {
        KISMET_ASSERT(index < rank);
        return index == 0 ? N1 : N2;
    }

    // get the total number of elements
    size_type size() const { return num; }

    pointer       data() { return &m_a[0][0];  }
    const_pointer data() const { return &m_a[0][0]; }

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
