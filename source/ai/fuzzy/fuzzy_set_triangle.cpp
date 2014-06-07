#include "kismet/ai/fuzzy/fuzzy_set_triangle.h"

namespace kismet
{
namespace fuzzy
{

fuzzy_set_triangle::fuzzy_set_triangle(float left, float mid, float right)
    : fuzzy_set(mid)
    , m_left{ left }
    , m_right{ right }
{
    KISMET_ASSERT(left <= mid && mid <= right);
}

float fuzzy_set_triangle::do_get_dom(float input) const
{
    if (input < m_left)
    {
        return 0.0f;
    }

    if (input < get_mean_max())
    {
        KISMET_ASSERT(m_left < get_mean_max());

        float k = 1.0f / (get_mean_max() - m_left);
        return k * (input - m_left);
    }

    if (input < m_right)
    {
        KISMET_ASSERT(get_mean_max() < m_right);

        float k = -1.0f / (m_right - get_mean_max());
        return k * (input - m_right);
    }

    if (input == get_mean_max())
    {
        return 1.0f;
    }

    return 0.0f;
}

} // namespace fuzzy
} // namespace kismet
