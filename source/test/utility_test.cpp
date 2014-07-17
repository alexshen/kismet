#include <boost/test/unit_test.hpp>
#include "kismet/is_comparable.h"
using namespace kismet;

BOOST_AUTO_TEST_CASE(is_comparable_test)
{
    struct not_comparable {};
    
    BOOST_CHECK((is_comparable<not_comparable, not_comparable>::value));
    BOOST_CHECK((is_comparable<int, int>::value));
}