#include "kismet/config.h"
#include "test/utility.h"

#ifdef KISMET_MSC
#  pragma warning(push)
#  pragma warning(disable: 4244)
#endif

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <random>
#include <type_traits>
#include <boost/test/unit_test.hpp>

#include "kismet/math/matrix.h"
#include "kismet/math/vector.h"

using namespace kismet::math;
using namespace std;

BOOST_AUTO_TEST_SUITE(matrix_test)

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
    
    BOOST_CHECK_EQUAL(m + m, 2.0f * m);
}

BOOST_AUTO_TEST_CASE(matrix22f_sub_self_equal_zero)
{
    matrix22f m
    {
        { 1, 0 },
        { 0, 1 },
    };
    
    BOOST_CHECK_EQUAL(m - m, 0.0f * m);
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
    m.row(0).assign(begin(v), end(v));
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
        BOOST_CHECK_EQUAL(m.column(i), m.column(i));
        BOOST_CHECK_EQUAL(cm.column(i), cm.column(i));
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
    BOOST_CHECK_EQUAL(m.column(0), m.column(1));
    BOOST_CHECK_EQUAL(cm.column(0), cm.column(1));
}

BOOST_AUTO_TEST_CASE(matrix22f_col_convertible_to_const_col)
{
    BOOST_CHECK((is_convertible<matrix22f::column_type, matrix22f::const_column_type>::value));
}

BOOST_AUTO_TEST_CASE(matrix22f_const_col_not_convertible_to_col)
{
    BOOST_CHECK(!(is_convertible<matrix22f::const_column_type, matrix22f::column_type>::value));
}

BOOST_AUTO_TEST_CASE(matrix22f_data_equal_init_data)
{
    float a[] = { 1, 2, 3, 4 };
    matrix22f m{begin(a), end(a)};
    matrix22f const& cm = m;

    BOOST_CHECK_EQUAL_COLLECTIONS(a, a + 4, m.data(), m.data() + m.size());
    BOOST_CHECK_EQUAL_COLLECTIONS(a, a + 4, cm.data(), cm.data() + cm.size());
}

BOOST_AUTO_TEST_CASE(matrix22f_equal_to_matrix22i)
{
    matrix<int, 2, 2> im
    {
        { 1, 0 },
        { 0, 1 }
    };

    matrix22f fm{im};
    BOOST_CHECK_EQUAL(fm, im);
}

BOOST_AUTO_TEST_CASE(matrix22f_not_equal_to_matrix22i)
{
    matrix<int, 2, 2> im
    {
        { 1, 0 },
        { 0, 1 }
    };

    matrix22f fm
    {
        { 1, 1 },
        { 0, 1 },
    };
    BOOST_CHECK(fm != im);
}

#ifndef KISMET_MSC
BOOST_AUTO_TEST_CASE(int_matrix_sclale_float_return_float_matrix)
{
    BOOST_CHECK((std::is_same<matrix22f, decltype(1.0f * matrix<int, 2, 2>{})>::value));
    BOOST_CHECK((std::is_same<matrix22f, decltype(matrix<int, 2, 2>{} * 1.0f)>::value));
}
#endif

BOOST_AUTO_TEST_CASE(matrix_row_mul)
{
    matrix<int, 1, 2> m { {1, 1} };
    float v[] = { 2, 2 };
    m[0] *= 2;
    BOOST_CHECK_EQUAL_COLLECTIONS(m[0].begin(), m[0].end(), v, v+2);
}

BOOST_AUTO_TEST_CASE(matrix_row_div)
{
    matrix<int, 1, 2> m { {2, 2} };
    float v[] = { 1, 1 };
    m[0] /= 2;
    BOOST_CHECK_EQUAL_COLLECTIONS(m[0].begin(), m[0].end(), v, v+2);
}

BOOST_AUTO_TEST_CASE(matrix_col_mul)
{
    matrix<int, 2, 1> m{ {1}, {1} };
    float v[] = { 2, 2 };
    m.column(0) *= 2;
    BOOST_CHECK_EQUAL_COLLECTIONS(m.column(0).begin(), m.column(0).end(), v, v+2);
}

BOOST_AUTO_TEST_CASE(matrix_col_div)
{
    matrix<int, 2, 1> m{ {2}, {2} };
    float v[] = { 1, 1 };
    m.column(0) /= 2;
    BOOST_CHECK_EQUAL_COLLECTIONS(m.column(0).begin(), m.column(0).end(), v, v+2);
}

BOOST_AUTO_TEST_CASE(matrix_identity_return_identity_matrix)
{
    matrix22f m1 = matrix22f::identity;
    matrix22f m2
    {
        { 1, 0 },
        { 0, 1 },
    };

    BOOST_CHECK_EQUAL(m1, m2);
}

BOOST_AUTO_TEST_CASE(matrix_swap_row)
{
    matrix22f m{ matrix22f::identity };
    matrix22f expected
    {
        { 0, 1 },
        { 1, 0 }
    };

    m.row(0).swap(m.row(1));

    BOOST_CHECK_EQUAL(m, expected);
}

BOOST_AUTO_TEST_CASE(matrix_swap_col)
{
    matrix22f m{ matrix22f::identity };
    matrix22f expected
    {
        { 0, 1 },
        { 1, 0 }
    };

    m.column(0).swap(m.column(1));

    BOOST_CHECK_EQUAL(m, expected);
}

BOOST_AUTO_TEST_CASE(matrix_swap_row_col)
{
    matrix22f m
    {
        { 1, 2 },
        { 3, 4 }
    };

    matrix22f expected
    {
        { 1, 3 },
        { 2, 4 }
    };

    m.row(0).swap(m.column(0));

    BOOST_CHECK_EQUAL(m, expected);
}

BOOST_AUTO_TEST_CASE(matrix_vector_row_normalize)
{
    using matrix_type = matrix<float, 1, 2>;
    matrix_type m
    {
        { 1, 1 },
    };

    matrix_type normalized
    {
        {1.0f / sqrt(2.0f), 1.0f / sqrt(2.0f)}
    };
    BOOST_CHECK(m.row(0).safe_normalize());
    KISMET_CHECK_APPROX_COLLECTIONS(m.row(0), normalized.row(0));
}

BOOST_AUTO_TEST_CASE(matrix_vector_row_mag)
{
    matrix22f m = matrix22f::identity;
    BOOST_CHECK_EQUAL(mag(m.row(0)), 1.f);
    BOOST_CHECK_EQUAL(squared_mag(m.row(0)), 1.f);
}

BOOST_AUTO_TEST_CASE(matrix_vector_column_normalize)
{
    using matrix_type = matrix<float, 2, 1>;
    matrix_type m
    {
        { 1 },
        { 1 }
    };

    matrix_type normalized
    {
        { 1.0f / sqrt(2.0f) }, 
        { 1.0f / sqrt(2.0f) }
    };
    BOOST_CHECK(m.column(0).safe_normalize());
    KISMET_CHECK_APPROX_COLLECTIONS(m.column(0), normalized.column(0));
}

BOOST_AUTO_TEST_CASE(matrix_vector_column_mag)
{
    matrix22f m = matrix22f::identity;
    BOOST_CHECK_EQUAL(mag(m.column(0)), 1.f);
    BOOST_CHECK_EQUAL(squared_mag(m.column(0)), 1.f);
}

BOOST_AUTO_TEST_CASE(matrix_row_iterator_conversion)
{
    BOOST_CHECK((is_convertible<matrix22f::row_iterator, matrix22f::const_row_iterator>::value));
    BOOST_CHECK((is_convertible<matrix22f::row_iterator, matrix22f::row_iterator>::value));
    BOOST_CHECK((is_convertible<matrix22f::const_row_iterator, matrix22f::const_row_iterator>::value));
    BOOST_CHECK((!is_convertible<matrix22f::const_row_iterator, matrix22f::row_iterator>::value));
}

BOOST_AUTO_TEST_CASE(matrix_row_iterator_difference)
{
    matrix22f m;

    auto beg = m.row_begin();
    auto end = m.row_end();

    BOOST_CHECK_EQUAL(end - beg, matrix22f::rank);
}

BOOST_AUTO_TEST_CASE(matrix_row_iterator_dereference)
{
    matrix22f m
    {
        { 1, 2 },
        { 3, 4 }
    };

    auto beg = m.row_begin();
    BOOST_CHECK_EQUAL(*beg, m[0]);
    ++beg;
    BOOST_CHECK_EQUAL(*beg, m[1]);
}

BOOST_AUTO_TEST_CASE(matrix_column_iterator_conversion)
{
    BOOST_CHECK((is_convertible<matrix22f::column_iterator, matrix22f::const_column_iterator>::value));
    BOOST_CHECK((is_convertible<matrix22f::column_iterator, matrix22f::column_iterator>::value));
    BOOST_CHECK((is_convertible<matrix22f::const_column_iterator, matrix22f::const_column_iterator>::value));
    BOOST_CHECK((!is_convertible<matrix22f::const_column_iterator, matrix22f::column_iterator>::value));
}

BOOST_AUTO_TEST_CASE(matrix_column_iterator_difference)
{
    matrix22f m;

    auto beg = m.column_begin();
    auto end = m.column_end();

    BOOST_CHECK_EQUAL(end - beg, 2);
}

BOOST_AUTO_TEST_CASE(matrix_column_iterator_dereference)
{
    matrix22f m
    {
        { 1, 2 },
        { 3, 4 }
    };

    auto beg = m.column_begin();
    BOOST_CHECK_EQUAL(*beg, m.column(0));
    ++beg;
    BOOST_CHECK_EQUAL(*beg, m.column(1));
}

BOOST_AUTO_TEST_CASE(matrix_transpose)
{
    matrix<float, 2, 3> m
    {
        { 1, 2, 3 },
        { 4, 5, 6 }
    };

    matrix<float, 3, 2> exp_t
    {
        { 1, 4 },
        { 2, 5 },
        { 3, 6 }
    };

    auto t = transpose(m);
    KISMET_CHECK_EQUAL_COLLECTIONS(t, exp_t);
}

BOOST_AUTO_TEST_CASE(matrix_inverse_2x2_succeeds)
{
    matrix22f m
    {
        { 1, 2 },
        { 4, 4 }
    };

    matrix22f exp_m
    {
        { -1, 0.5f },
        { 1, -0.25f }
    };

    auto inverse = invert(m);
    KISMET_CHECK_APPROX_COLLECTIONS(inverse, exp_m);
}

BOOST_AUTO_TEST_CASE(matrix_inverse_2x2_fails)
{
    matrix22f m
    {
        { 1, 2 },
        { 1, 2 }
    };

    matrix22f inverse;
    BOOST_CHECK(!invert(m, inverse));
}

BOOST_AUTO_TEST_CASE(matrix_inverse_3x3_succeeds)
{
    matrix33f m
    {
        { 1, 2, 4 },
        { 2, 2, 4 },
        { 4, 4, 4 }
    };

    matrix33f exp_inv
    {
        { -1, 1, 0 },
        { 1, -1.5f, 0.5f },
        { 0, 0.5f, -0.25f }
    };

    auto inverse = invert(m);
    KISMET_CHECK_EQUAL_COLLECTIONS(inverse, exp_inv);
}

BOOST_AUTO_TEST_CASE(matrix_inverse_3x3_fails)
{
    matrix33f m;
    m.clear();

    matrix33f inverse;
    BOOST_CHECK(!invert(m, inverse));
}

BOOST_AUTO_TEST_CASE(matrix_inverse_5x5_succeeds)
{
    matrix<float, 5, 5> m
    {
        { 1,   2,  4,  8, 16 },
        { 2,   2,  4,  8, 16 },
        { 4,   4,  4,  8, 16 },
        { 8,   8,  8,  8, 16 },
        { 16, 16, 16, 16, 16 }
    };

    matrix<float, 5, 5> exp_inv
    {
        { -1,     1,      0,       0,        0 },
        {  1, -1.5f,   0.5f,       0,        0 },
        {  0,  0.5f, -0.75f,   0.25f,        0 }, 
        {  0,     0,  0.25f, -0.375f,   0.125f },
        {  0,     0,      0,  0.125f, -0.0625f }
    };

    auto inverse = invert(m);
    KISMET_CHECK_EQUAL_COLLECTIONS(inverse, exp_inv);
}

namespace
{

template<typename T>
void random_matrix(T& m)
{
    mt19937 mt;
    uniform_real_distribution<float> dis;
    generate(m.begin(), m.end(), [&]{
        return dis(mt);
    });
}

}

BOOST_AUTO_TEST_CASE(matrix_mul)
{
    matrix33f m1(matrix33f::identity);
    matrix33f m2;
    random_matrix(m2);

    auto m3 = m1 * m2;
    BOOST_CHECK_EQUAL(m3, m2);
}

BOOST_AUTO_TEST_CASE(matrix_vector_assign)
{
    matrix33f m(matrix33f::identity);
    m[0][2] = 1.0f;
    m.row(0).assign(vector2f(1.0f, 1.0f));
    BOOST_CHECK_EQUAL(m.row(0), vector3f(1.0f, 1.0f, 1.0f));
}

BOOST_AUTO_TEST_SUITE_END()
