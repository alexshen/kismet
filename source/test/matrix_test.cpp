#include <cstddef>
#include <type_traits>
#include <boost/test/unit_test.hpp>
#include "kismet/math/matrix.h"
using namespace kismet::math;
using namespace std;

BOOST_AUTO_TEST_CASE(matrix_rank_2)
{
    BOOST_CHECK_EQUAL(matrix22f::rank, 2);
}

BOOST_AUTO_TEST_CASE(matrix22f_num_4)
{
    BOOST_CHECK_EQUAL(matrix22f::num, 4);
}

BOOST_AUTO_TEST_CASE(matrix22f_extent_is_2_2)
{
    matrix22f m;
    BOOST_CHECK_EQUAL(m.extent(0), 2);
    BOOST_CHECK_EQUAL(m.extent(1), 2);
}

BOOST_AUTO_TEST_CASE(matrix22f_equal_self)
{
    matrix22f m
    {
        { 1, 0 },
        { 0, 1 },
    };

    BOOST_CHECK_EQUAL(m, m);
}

BOOST_AUTO_TEST_CASE(matrix22f_add_self_equal_double_self)
{
    matrix22f m
    {
        { 1, 0 },
        { 0, 1 },
    };
    
    BOOST_CHECK_EQUAL(m + m, 2 * m);
}

BOOST_AUTO_TEST_CASE(matrix22f_sub_self_equal_zero)
{
    matrix22f m
    {
        { 1, 0 },
        { 0, 1 },
    };
    
    BOOST_CHECK_EQUAL(m - m, 0 * m);
}

BOOST_AUTO_TEST_CASE(matrix22f_row_equal_self)
{
    matrix22f m
    {
        { 1, 0 },
        { 0, 1 },
    };

    matrix22f const& cm = m;
    for (size_t i = 0; i < 2; ++i)
    {
        BOOST_CHECK_EQUAL(m.row(i), m.row(i));
        BOOST_CHECK_EQUAL(cm.row(i), cm.row(i));
    }
}

BOOST_AUTO_TEST_CASE(matrix22f_row_const_row_equal)
{
    matrix22f m
    {
        { 1, 0 },
        { 0, 1 },
    };

    matrix22f const& cm = m;
    for (size_t i = 0; i < 2; ++i)
    {
        BOOST_CHECK_EQUAL(m.row(i), cm.row(i));
        BOOST_CHECK_EQUAL(cm.row(i), m.row(i));
    }
}

BOOST_AUTO_TEST_CASE(matrix22f_row_assign_from_row)
{
    matrix22f m
    {
        { 1, 0 },
        { 0, 1 },
    };

    m.row(0) = m.row(1);
    BOOST_CHECK_EQUAL(m.row(0), m.row(1));
}

BOOST_AUTO_TEST_CASE(matrix22f_row_assign_from_array)
{
    matrix22f m
    {
        { 1, 0 },
        { 0, 1 },
    };

    float v[2] = {};
    m.row(0).assign(v);
    BOOST_CHECK_EQUAL(m.row(0), matrix22f::row_type(v));
}

BOOST_AUTO_TEST_CASE(matrix22f_row_convertible_to_const_row)
{
    BOOST_CHECK((is_convertible<matrix22f::row_type, matrix22f::const_row_type>::value));
}

BOOST_AUTO_TEST_CASE(matrix22f_const_row_not_convertible_to_row)
{
    BOOST_CHECK(!(is_convertible<matrix22f::const_row_type, matrix22f::row_type>::value));
}

BOOST_AUTO_TEST_CASE(matrix22f_col_equal_self)
{
    matrix22f m
    {
        { 1, 0 },
        { 0, 1 },
    };

    matrix22f const& cm = m;
    for (size_t i = 0; i < 2; ++i)
    {
        BOOST_CHECK_EQUAL(m.col(i), m.col(i));
        BOOST_CHECK_EQUAL(cm.col(i), cm.col(i));
    }
}

BOOST_AUTO_TEST_CASE(matrix22f_col_equal)
{
    matrix22f m
    {
        { 1, 1 },
        { 1, 1 },
    };

    matrix22f const& cm = m;
    BOOST_CHECK_EQUAL(m.col(0), m.col(1));
    BOOST_CHECK_EQUAL(cm.col(0), cm.col(1));
}

BOOST_AUTO_TEST_CASE(matrix22f_col_convertible_to_const_col)
{
    BOOST_CHECK((is_convertible<matrix22f::col_type, matrix22f::const_col_type>::value));
}

BOOST_AUTO_TEST_CASE(matrix22f_const_col_not_convertible_to_col)
{
    BOOST_CHECK(!(is_convertible<matrix22f::const_col_type, matrix22f::col_type>::value));
}


BOOST_AUTO_TEST_CASE(matrix22f_data_equal_init_data)
{
    float a[] = { 1, 2, 3, 4 };
    matrix22f m{a};
    matrix22f const& cm = a;

    BOOST_CHECK_EQUAL_COLLECTIONS(a, a + 4, m.data(), m.data() + m.size());
    BOOST_CHECK_EQUAL_COLLECTIONS(a, a + 4, cm.data(), cm.data() + cm.size());
}
