#ifndef KISMET_TEST_UTILITY_H
#define KISMET_TEST_UTILITY_H

#include <boost/test/unit_test.hpp>
#include <algorithm>
#include "kismet/math/math_trait.h"

namespace kismet
{

namespace test
{

struct approx_equal
{
    template<typename T>
    bool operator ()(T const& lhs, T const& rhs) const
    {
        return math::approx(lhs, rhs);
    }
};

template<typename InputIt1, typename InputIt2>
inline bool approx(InputIt1 start0, InputIt1 end0, InputIt2 start1)
{
    return std::equal(start0, end0, start1, approx_equal());
}

} // namespace test

} // namespace kismet

/// Check two collections are approximately equal
#define KISMET_CHECK_APPROX_COLLECTIONS(a, b) \
    BOOST_CHECK((kismet::test::approx(begin(a), end(a), begin(b))))

#define KISMET_CHECK_EQUAL_COLLECTIONS(a, b) \
    BOOST_CHECK_EQUAL_COLLECTIONS(begin(a), end(a), begin(b), end(b))

#endif // KISMET_TEST_UTILITY_H
