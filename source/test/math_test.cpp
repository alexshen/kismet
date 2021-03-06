#include <boost/test/unit_test.hpp>
#include <limits>
#include "kismet/math/math_trait.h"
using namespace kismet::math;
using namespace std;
namespace tt = boost::unit_test;

BOOST_AUTO_TEST_SUITE(math_test)

BOOST_AUTO_TEST_CASE(float_to_int_bits_test)
{
    const float_int<float>::int_type max_float_int = 0x7f7fffff;
    BOOST_CHECK_EQUAL(float_to_int_bits(numeric_limits<float>::max()), max_float_int);
    BOOST_CHECK_EQUAL(float_from_int_bits(max_float_int), numeric_limits<float>::max());
}

BOOST_AUTO_TEST_CASE(double_to_int_bits_test)
{
    const float_int<double>::int_type max_double_int = 0x7FEFFFFFFFFFFFFF;
    BOOST_CHECK_EQUAL(float_to_int_bits(numeric_limits<double>::max()), max_double_int);
    BOOST_CHECK_EQUAL(float_from_int_bits(max_double_int), numeric_limits<double>::max());
}

BOOST_AUTO_TEST_CASE(radian_to_degree)
{
    BOOST_TEST(180 == degrees(KISMET_PI_F));
}

BOOST_AUTO_TEST_CASE(int_degree_to_radian, * tt::tolerance(0.001f))
{
    BOOST_TEST(KISMET_PI_F == radians(180));
}

BOOST_AUTO_TEST_CASE(float_degree_to_radian, * tt::tolerance(0.001f))
{
    BOOST_TEST(KISMET_PI_F == radians(180.0f));
}

BOOST_AUTO_TEST_SUITE_END()
