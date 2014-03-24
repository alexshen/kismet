#include <boost/test/unit_test.hpp>
#include "kismet/math/matrix.h"
using namespace kismet::math;

BOOST_AUTO_TEST_CASE(matrix_1d_rank_1)
{
    BOOST_CHECK_EQUAL(matrix2f::rank, 1);
}

BOOST_AUTO_TEST_CASE(matrix2f_num_2)
{
    BOOST_CHECK_EQUAL(matrix2f::num, 2);
}

BOOST_AUTO_TEST_CASE(matrix_2d_rank_2)
{
    BOOST_CHECK_EQUAL(matrix22f::rank, 2);
}

BOOST_AUTO_TEST_CASE(matrix22f_num_4)
{
    BOOST_CHECK_EQUAL(matrix22f::num, 4);
}