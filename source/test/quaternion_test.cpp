#include <boost/test/unit_test.hpp>
#include "kismet/math/quaternion.h"

using namespace kismet;
using namespace math;

BOOST_AUTO_TEST_SUITE(quaternion_test)

BOOST_AUTO_TEST_CASE(quaternion_identity)
{
    BOOST_CHECK_EQUAL(quaternionf::identity, quaternionf(1.0f, 0.0, 0.0, 0.0f));
    BOOST_CHECK_EQUAL(mag(quaternionf::identity), 1.0f);
}

BOOST_AUTO_TEST_CASE(quaternion_rotate_vector)
{
    BOOST_CHECK_EQUAL(quaternionf::identity * vector3f::right  , vector3f::right);
    BOOST_CHECK_EQUAL(quaternionf::identity * vector3f::up     , vector3f::up);
    BOOST_CHECK_EQUAL(quaternionf::identity * vector3f::forward, vector3f::forward);

    quaternionf rot_around_z(vector3f::forward, deg2rad(90.0f));
    BOOST_CHECK_EQUAL(rot_around_z * vector3f::right, vector3f::up);

    quaternionf rot_around_y(vector3f::up, deg2rad(90.0f));
    BOOST_CHECK_EQUAL(rot_around_y * vector3f::right, -vector3f::forward);

    quaternionf rot_around_x(vector3f::right, deg2rad(90.0f));
    BOOST_CHECK_EQUAL(rot_around_x * vector3f::up, vector3f::forward);
}

BOOST_AUTO_TEST_SUITE_END()