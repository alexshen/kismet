#include "kismet/ai/fuzzy/fuzzy_set_right_trapezoid.h"
#include "kismet/core/assert.h"

namespace kismet
{
namespace fuzzy
{

fuzzy_set_right_trapezoid::fuzzy_set_right_trapezoid(float m1, float m2, float m3)
    : fuzzy_set((m1 + m2) * 0.5f)
    , m_m1{ m1 }
    , m_m2{ m2 }
    , m_m3{ m3 }
{
    KISMET_ASSERT(m1 <= m2 && m2 <= m3);
}

float fuzzy_set_right_trapezoid::do_get_dom(float input) const
{
    if (input < m_m1)
    {
        return 0.0f;
    }

    if (input <= m_m2)
    {
        if (m_m1 < m_m2)
        {
            float k = 1.0f / (m_m2 - m_m1);
            return k * (input - m_m1);
        }
        else
        {
            return 1.0f;
        }
    }

    if (input <= m_m3)
    {
        return 1.0f;
    }

    return 0.0f;
}

} // namespace fuzzy
} // namespace kismet
