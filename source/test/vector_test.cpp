#include <boost/test/unit_test.hpp>
#include <type_traits>
#include "kismet/math/matrix.h"
#include "kismet/math/vector.h"
#include "test/utility.h"

using namespace kismet::math;
using namespace std;

BOOST_AUTO_TEST_CASE(vector_scale)
{
    vector2f v = { 1.f, 2.f };
    vector2f doubled = { 2.f, 4.f };
    BOOST_CHECK_EQUAL(v * 2.f, doubled);
}

#ifndef KISMET_MSC
BOOST_AUTO_TEST_CASE(vector_float_scale_double_get_vector_double)
{
    BOOST_CHECK((is_same<decltype(vector2f{} *double{}), vector2d>::value));
}

BOOST_AUTO_TEST_CASE(vector_float_add_vector_double_get_vector_double)
{
    BOOST_CHECK((is_same<decltype(vector2f{} + vector2d{}), vector2d>::value));
}
#endif

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

BOOST_AUTO_TEST_CASE(vector_dot)
{
    vector2f v1(1, 2);
    vector2f v2(2, 3);
    float exp = v1[0] * v2[0] + v1[1] * v2[1];

    BOOST_CHECK_EQUAL((dot(v1, v2)), exp);
}

BOOST_AUTO_TEST_CASE(vector_cross)
{
    {
        vector3f v1(1, 2, 3);
        vector3f v2(4, 5, 6);
        vector3f exp(-3, 6, -3);

        BOOST_CHECK_EQUAL((cross(v1, v2)), exp);
    }

    {
        vector4f v1(1, 2, 3, 0);
        vector4f v2(4, 5, 6, 0);
        vector4f exp(-3, 6, -3, 0);

        BOOST_CHECK_EQUAL((cross(v1, v2)), exp);
    }
}

BOOST_AUTO_TEST_CASE(vector_left_mul)
{
    vector3f v(1, 0, 0);
    matrix33f m
    {
        { 1, 2, 3 },
        { 4, 5, 6 },
        { 7, 8, 9 }
    };
    vector3f exp(1, 2, 3);

    auto u = v * m;
    BOOST_CHECK_EQUAL(u, exp);
}

BOOST_AUTO_TEST_CASE(vector_right_mul)
{
    vector3f v(1, 0, 0);
    matrix33f m
    {
        { 1, 2, 3 },
        { 4, 5, 6 },
        { 7, 8, 9 }
    };
    vector3f exp(1, 4, 7);

    auto u = m * v;
    BOOST_CHECK_EQUAL(u, exp);
}

BOOST_AUTO_TEST_CASE(vector_normalize)
{
    vector3f res;
    BOOST_CHECK(!normalize(vector3f::zero, res));

    BOOST_CHECK((normalize(vector3f::right, res)));
    KISMET_CHECK_APPROX_COLLECTIONS(vector3f::right, res);
}

BOOST_AUTO_TEST_CASE(vector_mag)
{
    BOOST_CHECK_EQUAL(mag(vector3f::right), 1.f);
    BOOST_CHECK_EQUAL(squared_mag(vector3f::right), 1.f);
}
