#include <boost/test/unit_test.hpp>

#include "kismet/math/matrix.h"
#include "kismet/math/quaternion.h"
#include "kismet/math/transform.h"
#include "test/utility.h"

using namespace kismet;
using namespace math;

BOOST_AUTO_TEST_SUITE(quaternion_test)

BOOST_AUTO_TEST_CASE(matrix_rotation)
{
    matrix44<float> res = matrix_rotate(quaternionf::identity);
    KISMET_CHECK_APPROX_COLLECTIONS(res, matrix44<float>::identity);
}

BOOST_AUTO_TEST_CASE(matrix_to_quaternion)
{
    quaternionf res = matrix_to_quat(matrix44<float>::identity);
    KISMET_CHECK_APPROX_COLLECTIONS(res, quaternionf::identity);
}

BOOST_AUTO_TEST_SUITE_END()
