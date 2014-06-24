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

template<typename Derived, typename BaseIter, typename Category>
class strided_iterator_impl;

template<typename Derived, typename BaseIter>
class strided_iterator_impl<Derived, BaseIter, std::forward_iterator_tag>
    : public boost::iterator_adaptor<
                Derived,
                BaseIter,
                boost::use_default,
                std::forward_iterator_tag>
{
    friend class boost::iterator_core_access;
    using base_type = boost::iterator_adaptor<
                            Derived,
                            BaseIter,
                            boost::use_default,
                            std::forward_iterator_tag>;
public:
    using difference_type = typename std::iterator_traits<BaseIter>::difference_type;

    strided_iterator_impl() = default;

    strided_iterator_impl(BaseIter const& start, BaseIter const& cur, BaseIter const& end, difference_type s)
        : base_type(cur)
        , m_end(end)
        , m_stride(s)
    {
        KISMET_ASSERT(s > 0);
    }

    strided_iterator_impl(BaseIter const& start, BaseIter const& end, difference_type offset, difference_type s)
        : base_type(end)
        , m_start(start)
        , m_end(end)
        , m_stride(s)
    {
        // offset is not used
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

    template<typename BaseIter2>
    difference_type distance_to(strided_iterator_impl<Derived, BaseIter2, std::forward_iterator_tag> const& rhs) const
    {
        auto dist = std::distance(this->base(), rhs.base());
        return (dist + (m_stride - 1)) / m_stride;
    }

    BaseIter m_end;
    difference_type m_stride;
};

template<typename Derived, typename BaseIter>
class strided_iterator_impl<Derived, BaseIter, std::bidirectional_iterator_tag>
    : public boost::iterator_adaptor<
                Derived,
                BaseIter,
                boost::use_default,
                std::bidirectional_iterator_tag>
{
    friend class boost::iterator_core_access;
    using base_type = boost::iterator_adaptor<
                            Derived,
                            BaseIter,
                            boost::use_default,
                            std::bidirectional_iterator_tag>;
public:
    using difference_type = typename std::iterator_traits<BaseIter>::difference_type;

    strided_iterator_impl() = default;

    strided_iterator_impl(BaseIter const& start, BaseIter const& cur, BaseIter const& end, difference_type s)
        : base_type(cur)
        , m_start(start)
        , m_end(end)
        , m_offset(0)
        , m_stride(s)
    {
        KISMET_ASSERT(s > 0);
    }

    strided_iterator_impl(BaseIter const& start, BaseIter const& end, difference_type offset, difference_type s)
        : base_type(end)
        , m_start(start)
        , m_end(end)
        , m_offset(offset)
        , m_stride(s)
    {
        KISMET_ASSERT(0 <= offset && offset < s);
    }

    difference_type stride() const
    {
        return m_stride;
    }
private:
    void increment()
    {
        auto offset = m_offset != 0 ? 0 : m_stride;
        while (this->base_reference() != m_end && m_offset++ < offset)
        {
            ++this->base_reference();
        }

        if (m_offset == m_stride)
        {
            m_offset = 0;
        }
    }

    void decrement()
    {
        auto offset = m_offset != 0 ? m_offset : m_stride;
        while (this->base_reference() != m_start && offset--)
        {
            --it;
        }

        if (offset > 0)
        {
            m_offset = offset - m_stride;
        }
    }

    template<typename BaseIter2>
    difference_type distance_to(strided_iterator_impl<Derived, BaseIter2, std::bidirectional_iterator_tag> const& rhs) const
    {
        auto dist = std::distance(this->base(), rhs.base());
        return (dist + (m_stride - 1)) / m_stride;
    }

    BaseIter m_start;
    BaseIter m_end;
    // The offset from the last valid iterator, normally this 0
    // In case we advanced to the end, and want to back to the last valid iterator,
    // or we backed to the start, and want to forward to the last valid iterator,
    // we will take into account this value.
    difference_type m_offset;
    difference_type m_stride;
};

template<typename Derived, typename BaseIter>
class strided_iterator_impl<Derived, BaseIter, std::random_access_iterator_tag>
    : public boost::iterator_adaptor<
                Derived,
                BaseIter,
                boost::use_default,
                std::random_access_iterator_tag>
{
    friend class boost::iterator_core_access;
    using base_type = boost::iterator_adaptor<
                            Derived,
                            BaseIter,
                            boost::use_default,
                            std::random_access_iterator_tag>;
public:
    using difference_type = typename std::iterator_traits<BaseIter>::difference_type;

    strided_iterator_impl() = default;

    strided_iterator_impl(BaseIter const& start, BaseIter const& cur, BaseIter const& end, difference_type s)
        : base_type(cur)
        , m_start(start)
        , m_end(end)
        , m_offset(0)
        , m_stride(s)
    {
        KISMET_ASSERT(start <= cur && cur <= end);
        KISMET_ASSERT(s > 0);
    }

    strided_iterator_impl(BaseIter const& start, BaseIter const& end, difference_type offset, difference_type s)
        : base_type(end)
        , m_start(start)
        , m_end(end)
        , m_offset(offset)
        , m_stride(s)
    {
        KISMET_ASSERT(start <= end);
        KISMET_ASSERT(0 <= offset && offset < s);
    }

    difference_type stride() const
    {
        return m_stride;
    }
private:
    void increment()
    {
        auto& it = this->base_reference();
        if (m_offset != 0)
        {
            KISMET_ASSERT(m_offset < 0);
            it -= m_offset;
            m_offset = 0;
        }
        else
        {
            auto offset = m_end - it;
            if (offset < m_stride)
            {
                it = m_end;
                m_offset = offset;
            }
            else
            {
                it += m_stride;
            }
        }
    }

    void decrement()
    {
        auto& it = this->base_reference();
        if (m_offset != 0)
        {
            KISMET_ASSERT(m_offset > 0);
            it -= m_offset;
            m_offset = 0;
        }
        else
        {
            auto offset = it - m_start;
            if (offset < m_stride)
            {
                it = m_start;
                m_offset = -offset;
            }
            else
            {
                it -= m_stride;
            }
        }
    }

    void advance(difference_type n)
    {
        auto& it = this->base_reference();
        if (n < 0)
        {
            if (it != m_start)
            {
                KISMET_ASSERT(m_offset >= 0);

                if (m_offset > 0)
                {
                    ++n;
                }

                auto offset = n * m_stride - m_offset;
                auto dist = m_start - it;
                if (dist < offset)
                {
                    it += offset;
                    m_offset = 0;
                }
                else
                {
                    it = m_start;
                    dist += m_offset;
                    m_offset = dist - dist / m_stride * m_stride;
                }
            }
        }
        else
        {
            if (it != m_end)
            {
                KISMET_ASSERT(m_offset <= 0);

                if (m_offset < 0)
                {
                    --n;
                }

                auto offset = n * m_stride - m_offset;
                auto dist = m_end - it;
                if (offset < dist)
                {
                    it += offset;
                    m_offset = 0;
                }
                else
                {
                    it = m_end;
                    m_offset = (dist + m_offset) % m_stride;
                }
            }
        }
    }

    template<typename BaseIter2>
    difference_type distance_to(strided_iterator_impl<Derived, BaseIter2, std::random_access_iterator_tag> const& rhs) const
    {
        if (rhs.base() >= this->base())
        {
            return (rhs.base() - this->base() + (m_stride - 1)) / m_stride;
        }
        return (rhs.base() - this->base() - (m_stride - 1)) / m_stride;
    }

    BaseIter m_start;
    BaseIter m_end;
    // The index from the last valid iterator, normally this 0
    // In case we advanced to the end, and want to back to the last valid iterator,
    // or we backed to the start, and want to forward to the last valid iterator,
    // we will take into account this value.
    difference_type m_offset;
    difference_type m_stride;
};
} // namespace detail

// A iterator advances at a fixed step.
// As it may not valid to advance beyond one-pass-the-end iterator,
// we have to provide the end iterator for checking the end of sequence.
template<typename BaseIter>
class strided_iterator
    : public detail::strided_iterator_impl<
                strided_iterator<BaseIter>,
                BaseIter,
                typename std::iterator_traits<BaseIter>::iterator_category>
{
    using base_type = detail::strided_iterator_impl<
                        strided_iterator,
                        BaseIter,
                        typename std::iterator_traits<BaseIter>::iterator_category>;
public:
    using difference_type = typename std::iterator_traits<BaseIter>::difference_type;

    // BaseIter must be default constructible
    strided_iterator()
    {
    }

    strided_iterator(BaseIter const& start, BaseIter const& cur, BaseIter const& end, difference_type s)
        : base_type(start, cur, end, s)
    {
    }

    // This is used for creating an end iterator
    // offset is the extra value that relative to the last valid iterator (including the end iterator).
    // E.g.
    //    1...2..E
    // If the stride id 3, then the next element of 1 is 2. There's no next element for 2,
    // but advance beyond E is undefined as per the standard, we have to save the offset from last valid element
    // so that we can back to the last valid element, in this case it is 2, when we're at the end.
    strided_iterator(BaseIter const& start, BaseIter const& end, difference_type offset, difference_type s)
        : base_type(start, end, offset, s)
    {
    }
};

} // namespace kismet

#endif // KISMET_STRIDED_ITEARTOR_H

