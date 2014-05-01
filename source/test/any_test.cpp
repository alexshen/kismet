#include <boost/test/unit_test.hpp>
#include "kismet/any.h"

using namespace kismet;
using namespace std;

BOOST_AUTO_TEST_CASE(any_defautl_consturcted_empty)
{
    any empty;
    BOOST_CHECK(empty.empty());
}

BOOST_AUTO_TEST_CASE(any_small_optimization)
{
    int i = 1;
    any a(i);
    BOOST_CHECK_EQUAL(unsafe_any_cast<int>(a), i);
}

BOOST_AUTO_TEST_CASE(any_no_small_optimization)
{
    struct pod
    {
        double d;
        int i;
    } v;
    
    v.d = 1.0;
    v.i = 2;

    any a(v);

    pod* p = unsafe_any_cast<pod*>(a);
    BOOST_CHECK_EQUAL(p->d, 1.0);
    BOOST_CHECK_EQUAL(p->i, 2);
}

BOOST_AUTO_TEST_CASE(any_copy)
{
    int i = 1;
    any a(i);
    any b(a);
    BOOST_CHECK_EQUAL(unsafe_any_cast<int>(a), i);
    BOOST_CHECK_EQUAL(unsafe_any_cast<int>(b), i);
}

BOOST_AUTO_TEST_CASE(any_move)
{
    string s = "111";
    any a(s);
    any b(std::move(a));
    BOOST_CHECK(a.empty());
    BOOST_CHECK_EQUAL(unsafe_any_cast<string&>(b), "111");
}

BOOST_AUTO_TEST_CASE(any_cast_throw)
{
    any a(1);
    BOOST_CHECK_EXCEPTION(any_cast<double>(a), bad_any_cast, [](bad_any_cast const&) { return true; });
}

BOOST_AUTO_TEST_CASE(any_unsafe_cast_const)
{
    any const a(1);
    BOOST_CHECK_EQUAL(unsafe_any_cast<int>(a), 1);
}

BOOST_AUTO_TEST_CASE(any_swap)
{
    any a(1);
    any b(string("aaa"));

    swap(a, b);

    BOOST_CHECK_EQUAL(unsafe_any_cast<int>(b), 1);
    BOOST_CHECK_EQUAL(unsafe_any_cast<string>(a), string("aaa"));
}