#ifndef KISMET_FUZZY_TRAPEZOID_H
#define KISMET_FUZZY_TRAPEZOID_H

#include "kismet/ai/fuzzy/fuzzy_set.h"

namespace kismet
{
namespace fuzzy
{

/**
 * A trapezoidal fuzzy set which shape is as below
 *      _________
 *     /         \
 * ___/           \____
 */
class fuzzy_set_trapezoid : public fuzzy_set
{
public:
    fuzzy_set_trapezoid(float m1, float m2, float m3, float m4);
private:
    float do_get_dom(float input) const override;

    float m_m1;
    float m_m2;
    float m_m3;
    float m_m4;
};

} // namespace fuzzy
} // namespace kismet

#endif // !KISMET_FUZZY_TRAPEZOID_H
