#include <algorithm>
#include <utility>
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

BOOST_AUTO_TEST_CASE(linear_system_GE_solve_identity)
{
    matrix22f a{ matrix22f::identity() };
    matrix<float, 2, 1> b
    {
        { 1 },
        { 2 }
    };

    float x[2];
    float expected_x[] { 1, 2 };
    BOOST_CHECK(solve_partial_pivoting(a, b, x));

    BOOST_CHECK_EQUAL_COLLECTIONS(begin(x), end(x), begin(expected_x), end(expected_x));
}

BOOST_AUTO_TEST_CASE(linear_system_GE_solve_non_invertible_fail)
{
    matrix22f a
    {
        { 1, 0 },
        { 0, 0 }
    };
    matrix<float, 2, 1> b
    {
        { 1 },
        { 2 }
    };

    float x[2];
    BOOST_CHECK(!solve_partial_pivoting(a, b, x));
}

BOOST_AUTO_TEST_CASE(linear_system_GE_solve_with_permuation)
{
    matrix22f a
    {
        { 1, 0 },
        { 2, 1 }
    };
    matrix<float, 2, 1> b
    {
        { 1 },
        { 2 }
    };

    float x[2];
    float expected_x[] = { 1.f, 0.f };
    BOOST_CHECK(solve_partial_pivoting(a, b, x));

    BOOST_CHECK_EQUAL_COLLECTIONS(begin(x), end(x), begin(expected_x), end(expected_x));
}

BOOST_AUTO_TEST_CASE(linear_system_lu_decompose_succeeds)
{
    matrix33f a
    {
        { 1, -2, 3 },
        { 2, -5, 12 },
        { 0, 2, -10 }
    };

    matrix33f exp_l
    {
        { 1, 0, 0 },
        { 2, 1, 0 },
        { 0, -2, 1 }
    };

    matrix33f exp_u
    {
        { 1, -2, 3 },
        { 0, -1, 6 },
        { 0, 0, 2 }
    };

    matrix33f l, u;
    BOOST_CHECK(lu_decompose(a, l, u));

    equal(begin(l), end(l), begin(exp_l), [](float actual, float exp)
    {
        bool e = approx(actual, exp, math_trait<float>::zero_tolerance());
        BOOST_CHECK(e);
        return e;
    });

    equal(begin(u), end(u), begin(exp_u), [](float actual, float exp)
    {
        bool e = approx(actual, exp, math_trait<float>::zero_tolerance());
        BOOST_CHECK(e);
        return e;
    });
}

BOOST_AUTO_TEST_CASE(linear_system_lu_decompose_fails)
{
    matrix22f a
    {
        { 1, 2 },
        { 0, 0 },
    };

    matrix22f l, u;
    BOOST_CHECK(!lu_decompose(a, l, u));
}