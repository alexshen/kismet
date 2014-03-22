#include <boost/test/unit_test.hpp>
#include "kismet/math/matrix.h"
using namespace kismet::math;

BOOST_AUTO_TEST_CASE(matrix_init)
{
    matrix2f mf2{1, 2};
    BOOST_CHECK_EQUAL(mf2.size(), 2);
    BOOST_CHECK_EQUAL(matrix2f::rank, 1);
    BOOST_CHECK_EQUAL(matrix2f::num, 2);

    matrix3f mf3{1, 2, 3};
    BOOST_CHECK_EQUAL(mf3.size(), 3);
    BOOST_CHECK_EQUAL(matrix3f::rank, 1);
    BOOST_CHECK_EQUAL(matrix3f::num, 3);

    matrix4f mf4{1, 2, 3, 4};
    BOOST_CHECK_EQUAL(mf4.size(), 4);
    BOOST_CHECK_EQUAL(matrix4f::rank, 1);
    BOOST_CHECK_EQUAL(matrix4f::num, 4);

    matrix22f mf22{
        {1, 2},
        {3, 4}
    };
    BOOST_CHECK_EQUAL(mf22.size(), 4);
    BOOST_CHECK_EQUAL(matrix22f::rank, 2);
    BOOST_CHECK_EQUAL(matrix22f::num, 4);

    matrix33f mf33{
        {1, 2, 3},
        {4, 5, 6},
        {7, 8, 9}
    };
    BOOST_CHECK_EQUAL(mf33.size(), 9);
    BOOST_CHECK_EQUAL(matrix33f::rank, 2);
    BOOST_CHECK_EQUAL(matrix33f::num, 9);

    matrix44f mf44{
        {1, 2, 3, 4},
        {5, 6, 7, 8},
        {9, 10, 11, 12},
        {13, 14, 15, 16},
    };
    BOOST_CHECK_EQUAL(mf44.size(), 16);
    BOOST_CHECK_EQUAL(matrix44f::rank, 2);
    BOOST_CHECK_EQUAL(matrix44f::num, 16);
}
