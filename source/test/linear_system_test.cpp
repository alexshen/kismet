#include <algorithm>
#include <boost/test/unit_test.hpp>
#include "kismet/math/linear_system.h"
using namespace std;
using namespace kismet::math;

BOOST_AUTO_TEST_CASE(linear_system_solve2x2_determinant_zero_fail)
{
    // initialize an zero array
    float a[2][2] = {};
    float b[2] = {};
    float c[2];
    BOOST_CHECK(!solve(a, b, c));
}

BOOST_AUTO_TEST_CASE(linear_system_solve2x2_identity_all_one)
{
    // identity matrix
    float a[2][2] = {
        { 1, 0 },
        { 0, 1 }
    };
    float b[2] = { 1, 1 };
    float c[2];
    BOOST_CHECK(solve(a, b, c));
    BOOST_CHECK(equal(b, b + 2, c));
}

BOOST_AUTO_TEST_CASE(linear_system_solve3x3_determinant_zero_fail)
{
    // initialize an zero array
    float a[3][3] = {};
    float b[3] = {};
    float c[3];
    BOOST_CHECK(!solve(a, b, c));
}

BOOST_AUTO_TEST_CASE(linear_system_solve3x3_identity_all_one)
{
    // identity matrix
    float a[3][3] = {
        { 1, 0, 0 },
        { 0, 1, 0 },
        { 0, 0, 1 }
    };
    float b[3] = { 1, 1, 1 };
    float c[3];
    BOOST_CHECK(solve(a, b, c));
    BOOST_CHECK(equal(b, b + 3, c));
}
