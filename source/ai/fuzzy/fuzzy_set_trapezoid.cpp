#include "kismet/ai/fuzzy/fuzzy_set_trapezoid.h"
#include "kismet/core/assert.h"

namespace kismet
{
namespace fuzzy
{

fuzzy_set_trapezoid::fuzzy_set_trapezoid(float m1, float m2, float m3, float m4)
    : fuzzy_set((m2 + m3) * 0.5f)
    , m_m1{ m1 }
    , m_m2{ m2 }
    , m_m3{ m3 }
    , m_m4{ m4 }
{
    KISMET_ASSERT(m1 <= m2 && m2 <= m3 && m3 <= m4);
}

float fuzzy_set_trapezoid::do_get_dom(float input) const
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

    if (input < m_m4)
    {
        KISMET_ASSERT(m_m3 < m_m4);

        float k = -1.0f / (m_m4 - m_m3);
        return k * (input - m_m3);
    }

    return 0.0f;
}

} // namespace fuzzy
} // namespace kismet
