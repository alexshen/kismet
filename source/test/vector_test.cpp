#include <boost/test/unit_test.hpp>
#include <type_traits>
#include "kismet/math/vector.h"
using namespace kismet::math;
using namespace std;

BOOST_AUTO_TEST_CASE(vector_scale)
{
    vector2f v = { 1.f, 2.f };
    vector2f doubled = { 2.f, 4.f };
    BOOST_CHECK_EQUAL(v * 2.f, doubled);
}

BOOST_AUTO_TEST_CASE(vector_float_scale_double_get_vector_double)
{
    BOOST_CHECK((is_same<decltype(vector2f{} *double{}), vector2d>::value));
}

BOOST_AUTO_TEST_CASE(vector_float_add_vector_double_get_vector_double)
{
    BOOST_CHECK((is_same<decltype(vector2f{} + vector2d{}), vector2d>::value));
}

BOOST_AUTO_TEST_CASE(vector_add)
{
    vector2f v = { 1.f, 2.f };
    vector2f doubled = { 2.f, 4.f };
    BOOST_CHECK_EQUAL(v + v, doubled);
}

BOOST_AUTO_TEST_CASE(vector_minus)
{
    vector2f v = { 1.f, 2.f };
    BOOST_CHECK_EQUAL(v - v, vector2f::zero);
}