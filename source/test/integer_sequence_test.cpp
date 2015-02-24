#include <type_traits>
#include <boost/test/unit_test.hpp>
#include "kismet/integer_sequence.h"

using namespace std;
using namespace kismet;

BOOST_AUTO_TEST_SUITE(integer_sequence_test)

BOOST_AUTO_TEST_CASE(integer_sequence_empty)
{
    using seq_type = make_integer_sequence<int, 0>;
    BOOST_CHECK_EQUAL(seq_type::size, 0);
    BOOST_CHECK((is_same<seq_type, integer_sequence<int>>::value));
}

BOOST_AUTO_TEST_CASE(integer_sequence_single)
{
    using seq_type = make_integer_sequence<int, 1>;
    BOOST_CHECK_EQUAL(seq_type::size, 1);
    BOOST_CHECK((is_same<seq_type, integer_sequence<int, 0>>::value));
}

BOOST_AUTO_TEST_CASE(integer_sequence_several)
{
    using seq_type = make_integer_sequence<int, 3>;
    BOOST_CHECK((is_same<seq_type, integer_sequence<int, 0, 1, 2>>::value));
}

BOOST_AUTO_TEST_SUITE_END()