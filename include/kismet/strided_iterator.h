#ifndef KISMET_STRIDED_ITEARTOR_H
#define KISMET_STRIDED_ITEARTOR_H

#include <cstddef>
#include <iterator>
#include <boost/iterator/iterator_adaptor.hpp>

#include "kismet/core/assert.h"

namespace kismet
{
namespace detail
{

template<typename BaseIter, typename Category>
class strided_iterator_impl;

template<typename BaseIter>
class strided_iterator_impl<BaseIter, std::forward_iterator_tag>
    : boost::iterator_adaptor<
        strided_iterator_impl<BaseIter, std::forward_iterator_tag>,
        boost::use_default,
        std::forward_iterator_tag>
{
    friend class boost::iterator_core_access;
    using base_type = boost::iterator_adaptor<
                        strided_iterator_impl<BaseIter, std::forward_iterator_tag>,
                        boost::use_default,
                        std::forward_iterator_tag>;
public:
    using difference_type = typename std::iterator_traits<BaseIter>::difference_type;

    // as we can move back, it's valid to provide a default constructor
    // BaseIter must be default constructible and copy constructible
    strided_iterator_impl()
        : m_end()
        , m_stride()
    {
    }

    strided_iterator_impl(BaseIter const& start, BaseIter const& it, BaseIter const& end, difference_type s)
        : base_type(it)
        , m_end(end)
        , m_stride(s)
    {
        KISMET_ASSERT(s > 0);
    }

    difference_type stride() const
    {
        return m_stride;
    }
private:
    void increment()
    {
        difference_type s = m_stride;
        while (this->base_reference() != m_end && s--)
        {
            ++this->base_reference();
        }
    }

    BaseIter m_end;
    difference_type m_stride;
};

template<typename BaseIter>
class strided_iterator_impl<BaseIter, std::bidirectional_iterator_tag>
    : boost::iterator_adaptor<
        strided_iterator_impl<BaseIter, std::bidirectional_iterator_tag>,
        boost::use_default,
        std::bidirectional_iterator_tag>
{
    friend class boost::iterator_core_access;
    using base_type = boost::iterator_adaptor<
                        strided_iterator_impl<BaseIter, std::bidirectional_iterator_tag>,
                        boost::use_default,
                        std::bidirectional_iterator_tag>;
public:
    using difference_type = typename std::iterator_traits<BaseIter>::difference_type;

    strided_iterator_impl()
        : strided_iterator_impl(0)
    {
    }

    // BaseIter must be default constructible and copy constructible
    strided_iterator_impl(difference_type s)
        : strided_iterator_impl(BaseIter(), BaseIter(), BaseIter(), s)
    {
    }

    strided_iterator_impl(BaseIter const& start, BaseIter const& it, BaseIter const& end, difference_type s)
        : base_type(it)
        , m_start(start)
        , m_end(end)
        , m_stride(s)
    {
        KISMET_ASSERT(s > 0);
    }

    difference_type stride() const
    {
        return m_stride;
    }
private:
    void increment()
    {
        difference_type s = m_stride;
        while (this->base_reference() != m_end && s--)
        {
            ++this->base_reference();
        }
    }

    void decrement()
    {
        difference_type s = m_stride;
        while (this->base_reference() != m_start && s--)
        {
            --this->base_reference();
        }
    }

    BaseIter m_start;
    BaseIter m_end;
    difference_type m_stride;
};

template<typename BaseIter>
class strided_iterator_impl<BaseIter, std::random_access_iterator_tag>
    : boost::iterator_adaptor<
        strided_iterator_impl<BaseIter, std::random_access_iterator_tag>,
        boost::use_default,
        std::random_access_iterator_tag>
{
    friend class boost::iterator_core_access;
    using base_type = boost::iterator_adaptor<
                        strided_iterator_impl<BaseIter, std::random_access_iterator_tag>,
                        boost::use_default,
                        std::random_access_iterator_tag>;
public:
    using difference_type = typename std::iterator_traits<BaseIter>::difference_type;

    strided_iterator_impl()
        : strided_iterator_impl(0)
    {
    }

    // BaseIter must be default constructible and copy constructible
    strided_iterator_impl(difference_type s)
        : strided_iterator_impl(BaseIter(), BaseIter(), BaseIter(), s)
    {
    }

    strided_iterator_impl(BaseIter const& start, BaseIter const& it, BaseIter const& end, difference_type s)
        : base_type(it)
        , m_start(start)
        , m_end(end)
        , m_stride(s)
    {
        KISMET_ASSERT(s > 0);
    }

    difference_type stride() const
    {
        return m_stride;
    }
private:
    void increment()
    {
        auto& it = this->base_reference();
        if (m_end - this->base_reference() <= m_stride)
        {
            it = m_end;
        }
        else
        {
            it += m_stride;
        }
    }

    void decrement()
    {
        auto& it = this->base_reference();
        if (this->base_reference() - m_start <= m_stride)
        {
            it = m_start;
        }
        else
        {
            it -= m_stride;
        }
    }

    void advance(difference_type n)
    {
        difference_type offset = n * m_stride;
        auto& it = this->base_reference();
        if (offset < 0)
        {
            if (it - m_start <= offset)
            {
                it = m_start;
            }
            else
            {
                it -= offset;
            }
        }
        else
        {
            if (m_end - it <= offset)
            {
                it = m_end;
            }
            else
            {
                it += offset;
            }
        }
    }

    template<typename BaseIter2>
    difference_type distance_to(strided_iterator_impl<BaseIter2, std::random_access_iterator_tag> const& rhs) const
    {
        return (this->base() - rhs.base()) / m_stride;
    }

    BaseIter m_start;
    BaseIter m_end;
    difference_type m_stride;
};
} // namespace detail

// A iterator advances at a fixed step.
// As it may not valid to advance beyond one-pass-the-end iterator,
// we have to provide the end iterator for checking the end of sequence.
template<typename BaseIter>
class strided_iterator
    : detail::strided_iterator_impl<
        BaseIter,
        typename std::iterator_traits<BaseIter>::iterator_category>
{
    using base_type = detail::strided_iterator_impl<
                        BaseIter,
                        typename std::iterator_traits<BaseIter>::iterator_category>;
public:
    using difference_type = typename std::iterator_traits<BaseIter>::difference_type;

    // BaseIter must be default constructible
    strided_iterator()
    {
    }

    // BaseIter must be default constructible and copy constructible
    strided_iterator(difference_type s)
        : base_type(s)
    {
    }

    strided_iterator(BaseIter const& start, BaseIter const& it, BaseIter const& end, difference_type s)
        : base_type(start, it, end, s)
    {
    }
};

} // namespace kismet

#endif // KISMET_STRIDED_ITEARTOR_H

