#include "kismet/math/math_trait.h"

namespace kismet
{

namespace math
{

float math_trait<float>::s_zero_tol = 1e-6f;
double math_trait<double>::s_zero_tol = 1e-15;

} // namespace math

} // namespace kismet
