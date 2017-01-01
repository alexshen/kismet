#ifndef KISMET_MATH_MATRIX_H
#define KISMET_MATH_MATRIX_H

#include <algorithm>
#include <cstddef>
#include <initializer_list>
#include <iterator>
#include <numeric>
#include <ostream>
#include <type_traits>
#include <utility>

#include <boost/iterator/iterator_facade.hpp>

#include "kismet/common_type.h"
#include "kismet/core/assert.h"
#include "kismet/enable_if_convertible.h"
#include "kismet/is_comparable.h"
#include "kismet/math/detail/vector_common.h"
#include "kismet/math/linear_system.h"
#include "kismet/math/math_trait.h"
#include "kismet/strided_iterator.h"
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

template<std::size_t S, std::size_t... S2>
struct all_strides_one
    : boolean_constant_t<S == 1 && all_strides_one<S2...>::value>
{
};

template<std::size_t S>
struct all_strides_one<S> : boolean_constant_t<S == 1> {};

template<typename Derived, typename T, std::size_t N, std::size_t S>
class matrix_vector_base_impl
{
    template<typename D, typename U, std::size_t N1, std::size_t S1>
    friend class matrix_vector_base_impl;
public:
    using size_type              = std::size_t;
    using reference              = T&;
    using const_reference        = T const&;
    using pointer                = T*;
    using const_pointer          = T const*;
    using iterator               = strided_iterator<pointer>;
    using const_iterator         = strided_iterator<const_pointer>;

    template<typename U>
    matrix_vector_base_impl(U* p, U* end)
        : m_p(p)
        , m_end(end)
    {
        KISMET_ASSERT(p && end);
    }

    template<typename D, typename U>
    matrix_vector_base_impl(matrix_vector_base_impl<D, U, N, S> const& v)
        : matrix_vector_base_impl(v.m_p, v.m_end)
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

    Derived& operator *=(T k)
    {
        for (size_type i = 0; i < N; ++i)
        {
            (*this)[i] *= k;
        }
        return static_cast<Derived&>(*this);
    }

    Derived& operator /=(T k)
    {
        KISMET_ASSERT(!is_zero(k));
        *this *= invert(k);
        return static_cast<Derived&>(*this);
    }

    iterator begin() { return {m_p, m_p, m_end, S}; }
    iterator end()   { return {m_p, m_end, end_offset(), S}; }

    const_iterator begin() const { return {m_p, m_p, m_end, S}; }
    const_iterator end()   const { return {m_p, m_end, end_offset(), S}; }
private:
    typename iterator::difference_type end_offset() const
    {
        return (m_end - m_p) % N;
    }
protected:
    pointer m_p;     // point to the start address of the column
    pointer m_end;   // matrix end
};

// optimization for row vector, no need to store matrix start and end
// as the iterator is not strided.
template<typename Derived, typename T, std::size_t N>
class matrix_vector_base_impl<Derived, T, N, 1>
{
    template<typename D, typename U, std::size_t N1, std::size_t S1>
    friend class matrix_vector_base_impl;
public:
    using size_type              = std::size_t;
    using reference              = T&;
    using const_reference        = T const&;
    using pointer                = T*;
    using const_pointer          = T const*;
    using iterator               = pointer;
    using const_iterator         = const_pointer;

    template<typename U>
    explicit matrix_vector_base_impl(U* p)
        : m_p(p)
    {
        KISMET_ASSERT(p);
    }

    template<typename U>
    matrix_vector_base_impl(U* p, U* end)
        : m_p(p)
    {
        KISMET_ASSERT(p);
    }

    template<typename D, typename U>
    matrix_vector_base_impl(matrix_vector_base_impl<D, U, N, 1> const& v)
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

    Derived& operator *=(T k)
    {
        std::for_each(data(), data() + N, [k](T& v) { v *= k; });
        return static_cast<Derived&>(*this);
    }

    Derived& operator /=(T k)
    {
        KISMET_ASSERT(!is_zero(k));
        div_integral(k, std::is_integral<T>());
        return static_cast<Derived&>(*this);
    }

    // only row type has data() as column type storage is not contiguous
    pointer       data()       { return this->m_p; }
    const_pointer data() const { return this->m_p; }

    iterator begin() { return m_p; }
    iterator end()   { return m_p + N; }

    const_iterator begin() const { return m_p; }
    const_iterator end()   const { return m_p + N; }
private:
    void div_integral(T k, std::true_type)
    {
        std::for_each(data(), data() + N, [k](T& v) { v /= k; });
    }

    void div_integral(T k, std::false_type)
    {
        *this *= invert(k);
    }
protected:
    pointer m_p;
};

template<typename Derived, typename T, std::size_t N, std::size_t S>
class matrix_vector_base : public matrix_vector_base_impl<Derived, T, N, S>
{
    using base_type = matrix_vector_base_impl<Derived, T, N, S>;
public:
    template<typename U>
    explicit matrix_vector_base(U* p)
        : base_type(p)
    {
    }

    template<typename U>
    matrix_vector_base(U* p, U* end)
        : base_type(p, end)
    {
    }

    template<typename D, typename U>
    matrix_vector_base(matrix_vector_base<D, U, N, S> const& v)
        : base_type(v)
    {
    }
};

template<typename Derived, typename T>
class matrix_vector_base<Derived, T, 1, 1> : public matrix_vector_base_impl<Derived, T, 1, 1>
{
    using base_type = matrix_vector_base_impl<Derived, T, 1, 1>;
public:
    template<typename U>
    explicit matrix_vector_base(U* p)
        : base_type(p)
    {
    }

    template<typename U>
    matrix_vector_base(U* start, U* p, U* end)
        : base_type(start, p, end)
    {
    }

    template<typename D, typename U>
    matrix_vector_base(matrix_vector_base<D, U, 1, 1> const& v)
        : base_type(v)
    {
    }

    operator typename base_type::reference()
    {
        return *this->m_p;
    }

    operator typename base_type::const_reference() const
    {
        return *this->m_p;
    }

    // For direct assignment, conversion operator does not work in this case
    Derived& operator =(T v)
    {
        *this->m_p = v;
        return static_cast<Derived&>(*this);
    }
};

template<typename T, typename U, typename V>
using enable_if_assignable_t =
    typename std::enable_if<
        std::is_convertible<T, U>::value
        && !std::is_const<U>::value,
        V>::type;

template<typename T, typename U, typename V>
using enable_if_comparable_t =
    typename std::enable_if<is_comparable<T, U>::value, V>::type;

template<std::size_t N1, std::size_t N2, std::size_t N>
struct is_row_or_column_matrix_with_size
    : boolean_constant_t<(N1 == 1 || N2 == 1) && N1 * N2 == N>
{};

} // namespace detail

// forward declaration
template<typename T, std::size_t N1, std::size_t N2>
class matrix;

template<typename T, std::size_t N>
class vector;

// Represents a row or column of a given matrix.
// The reference must not outlive the referenced matrix.
// N is the size of the row or the column
// S is the stride between elements.
template<typename T, std::size_t N, std::size_t S>
class matrix_vector
    : public detail::matrix_vector_base<matrix_vector<T, N, S>, T, N, S>
{
    static_assert(S != 0, "stride cannot be 0");

    using base_type = detail::matrix_vector_base<matrix_vector, T, N, S>;
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
    explicit matrix_vector(U* p,
                      typename std::enable_if<
                            S == 1 &&
                            std::is_convertible<U*, pointer>::value
                        >::type* = 0)
        : base_type(p)
    {
    }

    template<typename U>
    matrix_vector(U* p, U* end, enable_if_convertible<U*, pointer>* = 0)
        : base_type(p, end)
    {
    }

    template<typename U>
    matrix_vector(matrix_vector<U, N, S> const& v,
                      enable_if_convertible_t<
                        typename matrix_vector<U, N, S>::pointer, pointer
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

    // scalar assignment operator only intended for matrix_vector<T, 1, 1>
    matrix_vector& operator =(T v)
    {
        static_assert(N == 1 && N == S, "Scalar assignment operator is only intended for matrix_vector<T, 1, 1>");
        return base_type::operator =(v);
    }

    matrix_vector& operator =(matrix_vector const& v)
    {
        static_assert(!std::is_const<T>::value, "Cannot assign to const vector ref");
        copy_row_row(v, detail::all_strides_one<S>());
        return *this;
    }

    // assign from a row/column matrix
    template<typename U, std::size_t N2, std::size_t S2>
    typename std::enable_if<
        std::is_convertible<U, T>::value &&
        detail::is_row_or_column_matrix_with_size<N2, S2, N>::value,
        matrix_vector&
    >::type
    operator =(matrix<U, N2, S2> const& m)
    {
        // column/row matrix are contiguous, so we only need to check if we're row vector
        copy_row_row(m, detail::all_strides_one<S>());
        return *this;
    }

    template<typename U>
    detail::enable_if_assignable_t<U, T, matrix_vector&>
    operator =(vector<U, N> const& v)
    {
        // vector is contiguous, so we only need to check if we're row vector
        copy_row_row(v, detail::all_strides_one<S>());
        return *this;
    }

    // assign from a row/column
    template<typename U, std::size_t S2>
    detail::enable_if_assignable_t<U, T, matrix_vector&>
    operator =(matrix_vector<U, N, S2> const& v)
    {
        copy_row_row(v, detail::all_strides_one<S, S2>());
        return *this;
    }

    // assign from a sequence, if size of sequence is N, then
    // elements in [offset, offset+N) are updated with the elements from the sequence
    template<typename Iter>
    detail::enable_if_assignable_t<
        typename std::iterator_traits<Iter>::value_type
      , T
      , matrix_vector&>
    assign(Iter beg, Iter end, size_type offset = 0)
    {
        std::copy(beg, end, this->begin() + offset);
        return *this;
    }

    // equivalent to assign(v.begin(), v.end(), offset)
    template<typename U, std::size_t N2>
    detail::enable_if_assignable_t<U, T, matrix_vector&>
    assign(vector<U, N2> const& v, size_type offset = 0)
    {
        KISMET_ASSERT(offset + N2 <= N);
        return assign(v.begin(), v.end(), offset);
    }

    // compare a row/column to a row/column
    template<typename U, std::size_t S2>
    typename detail::enable_if_comparable_t<T, U, bool>
    operator ==(matrix_vector<U, N, S2> const& v) const
    {
        return equal_row_row(v, detail::all_strides_one<S, S2>());
    }

    template<typename U>
    typename detail::enable_if_comparable_t<T, U, bool>
    operator ==(vector<U, N> const& v) const
    {
        return equal_row_row(v, detail::all_strides_one<S>());
    }

    template<typename U, std::size_t N2, std::size_t S2>
    typename std::enable_if<
        is_comparable<T, U>::value &&
        detail::is_row_or_column_matrix_with_size<N2, S2, N>::value,
        matrix_vector&
    >::type
    operator ==(matrix<U, N2, S2> const& v) const
    {
        return equal_row_row(v, detail::all_strides_one<S>());
    }

    void normalize(T tolerance = math_trait<T>::zero_tolerance())
    {
        detail::normalize(*this, tolerance);
    }

    bool safe_normalize(T tolerance = math_trait<T>::zero_tolerance())
    {
        return detail::safe_normalize(*this, tolerance);
    }

    template<std::size_t S2>
    void swap(matrix_vector<T, N, S2>& v)
    {
        using std::swap;

        for (size_type i = 0; i < N; ++i)
        {
            swap((*this)[i], v[i]);
        }
    }

    template<std::size_t S2>
    void swap(matrix_vector<T, N, S2>&& v)
    {
        swap(v);
    }
private:
    // copy row vector/row matrix to row vector
    template<typename U>
    void copy_row_row(U const& v, std::true_type)
    {
        std::copy(v.begin(), v.end(), this->begin());
    }

    // column vector is not very efficient, so manual copy
    template<typename U>
    void copy_row_row(U const& v, std::false_type)
    {
        for (size_type i = 0; i < N; ++i)
        {
            (*this)[i] = v[i];
        }
    }

    // compare row to row
    template<typename U>
    bool equal_row_row(U const& v, std::true_type) const
    {
        return std::equal(v.begin(), v.end(), this->begin());
    }

    // compare row to column, or column to column, or column to row
    template<typename U, std::size_t S2>
    bool equal_row_row(matrix_vector<U, N, S2> const& v, std::false_type) const
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
};

template<typename T, std::size_t N, std::size_t S>
inline typename T::value_type squared_mag(matrix_vector<T, N, S> const& v)
{
    return detail::squared_mag(v);
}

template<typename T, std::size_t N, std::size_t S>
inline typename T::value_type mag(matrix_vector<T, N, S> const& v)
{
    return detail::mag(v);
}

template<typename T, std::size_t N, std::size_t S1, std::size_t S2>
inline void swap(matrix_vector<T, N, S1>& v1, matrix_vector<T, N, S2>& v2)
{
    v1.swap(v2);
}

template<typename T, std::size_t N, std::size_t S1, std::size_t S2>
inline void swap(matrix_vector<T, N, S1>&& v1, matrix_vector<T, N, S2>&& v2)
{
    v1.swap(v2);
}

template<typename T, std::size_t N, std::size_t S1, std::size_t S2>
inline void swap(matrix_vector<T, N, S1>& v1, matrix_vector<T, N, S2>&& v2)
{
    v1.swap(v2);
}

template<typename T, std::size_t N, std::size_t S1, std::size_t S2>
inline void swap(matrix_vector<T, N, S1>&& v1, matrix_vector<T, N, S2>& v2)
{
    v1.swap(v2);
}

template<typename T, std::size_t N1, std::size_t N2>
std::ostream& operator <<(std::ostream& os, matrix_vector<T, N1, N2> const& v)
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

namespace detail
{
// Enable if From is non const and To is const and
// From* is convertible to To*
template<typename From, typename To, typename T=void>
struct enable_if_non_const_to_const :
    std::enable_if<
        std::is_same<
            typename std::decay<From>::type
          , typename std::decay<To>::type
        >::value
     && std::is_convertible<
            typename std::add_pointer<From>::type
          , typename std::add_pointer<To>::type
        >::value
     , T
    >
{
};
}

/// Matrix row iterator
/// N is the number of elements per row
template<typename T, std::size_t N>
class matrix_row_iterator
    : public boost::iterator_facade<
        matrix_row_iterator<T, N>,
        matrix_vector<T, N, 1>,
        std::random_access_iterator_tag,
        matrix_vector<T, N, 1>
      >
{
    static_assert(N > 0, "N must be positive");

    using base_type = boost::iterator_facade<
        matrix_row_iterator<T, N>,
        matrix_vector<T, N, 1>,
        std::random_access_iterator_tag,
        matrix_vector<T, N, 1>
    >;

    using vector_type = matrix_vector<T, N, 1>;

    template<typename U, std::size_t S>
    friend class matrix_row_iterator;
public:
    using difference_type = typename base_type::difference_type;
    using reference       = typename base_type::reference;

    matrix_row_iterator()
        : m_data(nullptr)
    {
    }

    matrix_row_iterator(T* p)
        : m_data(p)
    {
        KISMET_ASSERT(p);
    }

    // for converting from non-const row_iterator
    template<typename U>
    matrix_row_iterator(
        matrix_row_iterator<U, N> const& rhs,
        typename detail::enable_if_non_const_to_const<U, T>::type* = 0)
        : m_data(rhs.m_data)
    {
    }

    // for converting from non-const row_iterator
    template<typename U>
    typename detail::enable_if_non_const_to_const<
        U, T, matrix_row_iterator&> operator =(matrix_row_iterator<U, N> const& rhs)
    {
        m_data = rhs.m_data;
        return *this;
    }
private:
    friend class boost::iterator_core_access;

    reference dereference() const
    {
        return vector_type(m_data);
    }

    template<typename T2>
    bool equal(matrix_row_iterator<T2, N> const& rhs) const
    {
        return m_data == rhs.m_data;
    }

    void increment()
    {
        m_data += N;
    }

    void decrement()
    {
        m_data -= N;
    }

    void advance(difference_type n)
    {
        m_data += n * N;
    }

    template<typename T2>
    difference_type distance_to(matrix_row_iterator<T2, N> const& rhs) const
    {
        auto dist = rhs.m_data - m_data;
        KISMET_ASSERT(std::abs(dist) % N == 0);
        return dist / static_cast<difference_type>(N);
    }

    T* m_data;
};

/// Matrix column iterator
/// N1 is the number of rows
/// N2 is the number of columns
template<typename T, std::size_t N1, std::size_t N2>
class matrix_column_iterator
    : public boost::iterator_facade<
        matrix_column_iterator<T, N1, N2>,
        matrix_vector<T, N1, N2>,
        std::random_access_iterator_tag,
        matrix_vector<T, N1, N2>
      >
{
    static_assert(N1 > 0 && N2 > 0, "N1 and N2 must be positive");

    using base_type = boost::iterator_facade<
        matrix_column_iterator<T, N1, N2>,
        matrix_vector<T, N1, N2>,
        std::random_access_iterator_tag,
        matrix_vector<T, N1, N2>
    >;

    using vector_type = matrix_vector<T, N1, N2>;

    template<typename U, std::size_t S1, std::size_t S2>
    friend class matrix_column_iterator;
public:
    using difference_type = typename base_type::difference_type;
    using reference       = typename base_type::reference;

    matrix_column_iterator()
        : m_data(nullptr)
        , m_end(nullptr)
    {
    }

    matrix_column_iterator(T* p, T* end)
        : m_data(p)
        , m_end(end)
    {
        KISMET_ASSERT(p && end && p <= end);
        KISMET_ASSERT(static_cast<std::size_t>(end - p) >= N1);
    }

    // for converting from non-const row_iterator
    template<typename U>
    matrix_column_iterator(
        matrix_column_iterator<U, N1, N2> const& rhs,
        typename detail::enable_if_non_const_to_const<U, T>::type* = 0)
        : m_data(rhs.m_data)
        , m_end(rhs.m_end)
    {
    }

    // for converting from non-const row_iterator
    template<typename U>
    typename detail::enable_if_non_const_to_const<
        U, T, matrix_column_iterator&> operator =(matrix_column_iterator<U, N1, N2> const& rhs)
    {
        m_data = rhs.m_data;
        m_end = rhs.m_end;
        return *this;
    }
private:
    friend class boost::iterator_core_access;

    reference dereference() const
    {
        return vector_type(m_data, m_end);
    }

    template<typename T2>
    bool equal(matrix_column_iterator<T2, N1, N2> const& rhs) const
    {
        return m_data == rhs.m_data && m_end == rhs.m_end;
    }

    void increment()
    {
        ++m_data;
    }

    void decrement()
    {
        --m_data;
    }

    void advance(difference_type n)
    {
        m_data += n;
    }

    template<typename T2>
    difference_type distance_to(matrix_column_iterator<T2, N1, N2> const& rhs) const
    {
        return rhs.m_data - m_data;
    }

    T* m_data;
    T* m_end; // matrix end
};

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

    using row_type              = matrix_vector<T, N2, 1>;
    using const_row_type        = matrix_vector<T const, N2, 1>;
    using column_type           = matrix_vector<T, N1, N2>;
    using const_column_type     = matrix_vector<T const, N1, N2>;
    using row_iterator          = matrix_row_iterator<T, N2>;
    using const_row_iterator    = matrix_row_iterator<T const, N2>;
    using column_iterator       = matrix_column_iterator<T, N1, N2>;
    using const_column_iterator = matrix_column_iterator<T const, N1, N2>;

    enum { rank = 2, num = N1 * N2 };

    matrix() = default;
    
    matrix(detail::matrix_initializer<T, rank> const& mi)
    {
        detail::insert_flat(data(), mi);
    }

    template<typename U>
    matrix(matrix<U, N1, N2> const& m,
           enable_if_convertible_t<U, T>* = 0)
    {
        *this = m;
    }

    // initialize from a range which starts by the given iterator
    template<typename Iter>
    explicit matrix(Iter beg, Iter end
    // vc does not compile, so disable this SFINAE code
#ifndef KISMET_MSC
                    , enable_if_convertible_t<
                        typename std::iterator_traits<Iter>::value_type, T>* = 0
#endif
                    )
    {
        assign(beg, end);
    }

    template<typename Iter>
    enable_if_convertible_t<
        typename std::iterator_traits<Iter>::value_type, 
        T, 
        matrix>&
    assign(Iter beg, Iter end)
    {
        checked_copy(beg, end, num, begin(), T(0));
        return *this;
    }

    template<typename U>
    enable_if_convertible_t<U, T, matrix>& operator =(matrix<U, N1, N2> const& m)
    {
        std::copy(m.data(), m.data() + num, data());
        return *this;
    }

    matrix& operator =(detail::matrix_initializer<T, rank> const& mi)
    {
        detail::insert_flat(data(), mi);
        return *this;
    }

    // assign a row/column vector to row/column matrix
    template<typename U, std::size_t S>
    typename std::enable_if<
        std::is_convertible<U, T>::value &&
        (N1 == 1 || N2 == 1),
        matrix&>::type
    operator =(matrix_vector<U, num, S> const& v)
    {
        copy_row_row(v, boolean_constant_t<N1 == 1>());
        return *this;
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
        KISMET_ASSERT(!is_zero(k));
        return *this *= invert(k);
    }

    matrix& operator *=(matrix<T, N2, N2> const& rhs)
    {
        matrix tmp;
        for (std::size_t i = 0; i < N1; ++i)
        {
            for (std::size_t j = 0; j < N2; ++j)
            {
                tmp[i][j] = T(0);
                for (std::size_t k = 0; k < N2; ++k)
                {
                    tmp[i][j] += m_a[i][k] * rhs.m_a[k][j];
                }
            }
        }
        *this = tmp;
        return *this;
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
    column_type column(size_type index)
    {
        KISMET_ASSERT(index < N2);
        return column_type{ &m_a[0][index], data() + num };
    }

    // Return the column object
    const_column_type column(size_type index) const
    {
        KISMET_ASSERT(index < N2);
        return const_column_type{ &m_a[0][index], data() + num };
    }

    // reset all elements to 0
    void clear()
    {
        std::fill_n(begin(), size(), T(0));
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

    row_iterator row_begin() { return { data() }; }
    row_iterator row_end() { return { data() + size() }; }

    const_row_iterator row_begin() const { return { data() }; }
    const_row_iterator row_end() const { return { data() + size() }; }

    column_iterator column_begin() { return { data(), data() + size() }; }
    column_iterator column_end() { return { data() + N2, data() + size() }; }

    const_column_iterator column_begin() const { return { data(), data() + size() }; }
    const_column_iterator column_end() const { return { data() + N2, data() + size() }; }

    static matrix const identity;
private:
    // copy row vector to row matrix
    template<typename U>
    void copy_row_row(U const& v, std::true_type)
    {
        row(0) = v;
    }

    // copy row/column vector to column matrix
    template<typename U>
    void copy_row_row(U const& v, std::false_type)
    {
        column(0) = v;
    }

    T m_a[N1][N2];
};

namespace detail
{

template<typename T, std::size_t N1, std::size_t N2>
struct identity_impl
{
    static_assert(N1 == N2, "identity only available for square matrix");
    static matrix<T, N1, N2> get()
    {
        matrix<T, N1, N2> m;
        for (std::size_t i = 0; i < N1; ++i)
        {
            for (std::size_t j = 0; j < N2; ++j)
            {
                m.data()[i * N2 + j] = i == j ? T(1) : T(0);
            }
        }
        return m;
    }
};

} // namespace detail

template<typename T, std::size_t N1, std::size_t N2>
matrix<T, N1, N2> const matrix<T, N1, N2>::identity = detail::identity_impl<T, N1, N2>::get();

/// Return the transpose of the given matrix
template<typename T, std::size_t N1, std::size_t N2>
inline matrix<T, N2, N1> transpose(matrix<T, N1, N2> const& m)
{
    matrix<T, N2, N1> t;

    for (std::size_t i = 0; i < N2; ++i)
    {
        t[i] = m.column(i);
    }

    return t;
}

namespace detail
{

template<typename T, std::size_t N>
struct inv_impl
{
    using matrix_type = matrix<T, N, N>;

    static bool calc(matrix_type const& a, matrix_type& inverse, T tolerance)
    {
        matrix_type l, u;
        std::size_t perm[N];

        plu_decompose(a, perm, l, u, tolerance);
        inverse.clear();
        // To calculate the inverse matrix A^-1, we can solve N linear systems which are of form
        //    A*xi = ei
        // where xi is the i-th column of the inverse matrix,
        // ei is the i-th column of the identity matrix.
        // Since
        //    A = PLU
        // we need to solve
        //    PLU*xi = ei
        matrix<T, N, 1> solution;
        for (std::size_t i = 0; i < N; ++i)
        {
            auto col = inverse.column(i);
            solution = col;

            using std::begin;
            using std::end;
            // let zi = LU*xi
            // solve P*zi = ei
            // we need to find the position of e[i][i] after permutation
            // we have zi = P^-1 * ei = P^T * ei
            // since we know p[m][p[m]] = 1, so to find j where
            // P^T[j][i] = 1, we can simply find j where
            // P[i][j] = 1, which means j = p[i].
            solution[perm[i]] = T(1);

            // let yi = U*xi
            // solve L*yi = zi
            if (!forward_substitute(l, solution, solution, tolerance))
            {
                return false;
            }

            // solve U*xi = yi
            if (!backward_substitute(u, solution, solution, tolerance))
            {
                return false;
            }

            col = solution;
        }

        return true;
    }
};

template<typename T>
struct inv_impl<T, 1>
{
    using matrix_type = matrix<T, 1, 1>;

    static bool calc(matrix_type a, matrix_type& inverse, T tolerance)
    {
        if (is_zero(a[0], tolerance))
        {
            return false;
        }
        a[0] = invert(a[0]);
        return true;
    }
};

template<typename T>
struct inv_impl<T, 2>
{
    using matrix_type = matrix<T, 2, 2>;

    static bool calc(matrix_type const& a, matrix_type& inverse, T tolerance)
    {
        T det = a[0][0] * a[1][1] - a[1][0] * a[0][1];
        if (is_zero(det, tolerance))
        {
            return false;
        }

        T inv_det = invert(det);
        inverse[0][0] =  a[1][1] * inv_det;
        inverse[0][1] = -a[0][1] * inv_det;
        inverse[1][0] = -a[1][0] * inv_det;
        inverse[1][1] =  a[0][0] * inv_det;

        return true;
    }
};

template<typename T>
struct inv_impl<T, 3>
{
    using matrix_type = matrix<T, 3, 3>;

    static bool calc(matrix_type const& a, matrix_type& inverse, T tolerance)
    {
        // Calculate the adjoint matrix
        inverse[0][0] = a[1][1] * a[2][2] - a[1][2] * a[2][1];
        inverse[0][1] = a[0][2] * a[2][1] - a[0][1] * a[2][2];
        inverse[0][2] = a[0][1] * a[1][2] - a[0][2] * a[1][1];
        inverse[1][0] = a[1][2] * a[2][0] - a[1][0] * a[2][2];
        inverse[1][1] = a[0][0] * a[2][2] - a[0][2] * a[2][0];
        inverse[1][2] = a[0][2] * a[1][0] - a[0][0] * a[1][2];
        inverse[2][0] = a[1][0] * a[2][1] - a[1][1] * a[2][0];
        inverse[2][1] = a[0][1] * a[2][0] - a[0][0] * a[2][1];
        inverse[2][2] = a[0][0] * a[1][1] - a[0][1] * a[1][0];

        // Calculate the determinant of a
        T det = a[0][0] * inverse[0][0] + a[0][1] * inverse[1][0] + a[0][2] * inverse[2][0];
        if (is_zero(det, tolerance))
        {
            return false;
        }

        inverse *= invert(det);
        return true;
    }
};

} // namespace detail

/// Calculate the inverse of the matrix using PLU decomposition
/// Return true if the matrix is invertible
/// NOTE: whether inverse is modified on failure is unspecified.
template<typename T, std::size_t N>
inline bool invert(matrix<T, N, N> const& a, matrix<T, N, N>& inverse, T tolerance = math_trait<T>::zero_tolerance())
{
    return detail::inv_impl<T, N>::calc(a, inverse, tolerance);
}

/// Calculate the inverse of the matrix using PLU decomposition
/// If the matrix is not invertible, original matrix is returned
template<typename T, std::size_t N>
inline matrix<T, N, N> invert(matrix<T, N, N> const& a, T tolerance = math_trait<T>::zero_tolerance())
{
    matrix<T, N, N> inverse;
    return invert(a, inverse, tolerance) ? inverse : a;
}

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

#ifndef KISMET_MSC
// return a matrix with element is common type of scalar and matrix's element type
template<typename U, typename T, std::size_t N1, std::size_t N2>
inline matrix<common_type_t<U, T>, N1, N2> operator *(U k, matrix<T, N1, N2> const& m)
{
    matrix<common_type_t<U, T>, N1, N2> res{m};
    res *= k;
    return res;
}
#endif

// TODO: do we need this for performance reason, in case that generic scalar multiplication version
// cannot optimize for rvalue ?
template<typename T, std::size_t N1, std::size_t N2>
inline matrix<T, N1, N2> operator *(T k, matrix<T, N1, N2> m)
{
    m *= k;
    return m;
}

#ifndef KISMET_MSC
// return a matrix with element is common type of scalar and matrix's element type
template<typename U, typename T, std::size_t N1, std::size_t N2>
inline matrix<common_type_t<U, T>, N1, N2> operator *(matrix<T, N1, N2> const& m, U k)
{
    matrix<common_type_t<U, T>, N1, N2> res{m};
    res *= k;
    return res;
}
#endif

// TODO: do we need this for performance reason, in case that generic scalar multiplication version
// cannot optimize for rvalue ?
template<typename T, std::size_t N1, std::size_t N2>
inline matrix<T, N1, N2> operator *(matrix<T, N1, N2> m, T k)
{
    m *= k;
    return m;
}

#ifndef KISMET_MSC
template<typename U, typename T, std::size_t N1, std::size_t N2>
inline matrix<common_type_t<U, T>, N1, N2> operator /(matrix<T, N1, N2> const& m, U k)
{
    matrix<common_type_t<U, T>, N1, N2> res{m};
    res /= k;
    return res;
}
#endif

// TODO: do we need this for performance reason, in case that generic scalar multiplication version
// cannot optimize for rvalue ?
template<typename T, std::size_t N1, std::size_t N2>
inline matrix<T, N1, N2> operator /(matrix<T, N1, N2> m, T k)
{
    m /= k;
    return m;
}

/// Matrix multiplication
///    M1 * M2
/// M1 which is N1 by N2
/// M2 which is N2 by N3
template<typename T, std::size_t N1, std::size_t N2, std::size_t N3>
inline matrix<T, N1, N3> operator *(matrix<T, N1, N2> const& m1, matrix<T, N2, N3> const& m2)
{
    matrix<T, N1, N3> res;
    for (std::size_t i = 0; i < N1; ++i)
    {
        for (std::size_t j = 0; j < N3; ++j)
        {
            res[i][j] = T(0);
            for (std::size_t k = 0; k < N2; ++k)
            {
                res[i][j] += m1[i][k] * m2[k][j];
            }
        }
    }
    return res;
}

template<typename T, typename U, std::size_t N1, std::size_t N2>
inline bool operator ==(matrix<T, N1, N2> const& m1, matrix<U, N1, N2> const& m2)
{
    return std::equal(m1.begin(), m1.end(), m2.begin());
}

template<typename T, typename U, std::size_t N1, std::size_t N2>
inline bool operator !=(matrix<T, N1, N2> const& m1, matrix<U, N1, N2> const& m2)
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

#define matrix_alias(d) \
    template<typename T> \
    using matrix##d##d = matrix<T, d, d>; \
    using matrix##d##d##f = matrix<float, d, d>; \
    using matrix##d##d##d = matrix<double, d, d>;

matrix_alias(2)
matrix_alias(3)
matrix_alias(4)

#undef matrix_alias

} // namespace math
} // namespace kismet

#endif // KISMET_MATH_matrix_H
