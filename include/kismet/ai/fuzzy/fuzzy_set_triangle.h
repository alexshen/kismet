#ifndef KISMET_FUZZY_SET_TRIANGLE_H
#define KISMET_FUZZY_SET_TRIANGLE_H

#include "kismet/ai/fuzzy/fuzzy_set.h"

namespace kismet
{
namespace fuzzy
{

/**
 * A triangular fuzzy set
 */
class fuzzy_set_triangle : public fuzzy_set
{
public:
    fuzzy_set_triangle(float left, float mid, float right);
private:
    float do_get_dom(float input) const override;

    /// The left most point of the triangle set
    float m_left;
    /// The right most point of the triangle set
    float m_right;
};

} // namespace fuzzy
} // namespace kismet

#endif // !KISMET_FUZZY_SET_TRIANGLE_H
