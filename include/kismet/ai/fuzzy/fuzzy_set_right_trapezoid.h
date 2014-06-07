#ifndef KISMET_FUZZY_SET_RIGHT_TRAPEZOID_H
#define KISMET_FUZZY_SET_RIGHT_TRAPEZOID_H

#include "kismet/ai/fuzzy/fuzzy_set.h"

namespace kismet
{
namespace fuzzy
{

/**
 * A trapezoidal fuzzy set which right side is vertical.
 *    m2_______m3
 *     /       
 *  m1/        
 * 
 */
class fuzzy_set_right_trapezoid : public fuzzy_set
{
public:
    fuzzy_set_right_trapezoid(float m1, float m2, float m3);
private:
    float do_get_dom(float input) const override;

    float m_m1;
    float m_m2;
    float m_m3;
};

} // namespace fuzzy
} // namespace kismet

#endif // !KISMET_FUZZY_SET_RIGHT_TRAPEZOID_H
