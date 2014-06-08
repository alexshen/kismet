#include <algorithm>
#include <limits>

#include "kismet/ai/fuzzy/fuzzy_variable.h"
#include "kismet/ai/fuzzy/fuzzy_set_triangle.h"
#include "kismet/ai/fuzzy/fuzzy_set_trapezoid.h"
#include "kismet/ai/fuzzy/fuzzy_set_left_trapezoid.h"
#include "kismet/ai/fuzzy/fuzzy_set_right_trapezoid.h"
#include "kismet/ai/fuzzy/fuzzy_set_singleton.h"
#include "kismet/math/math_trait.h"

using namespace std;

namespace kismet
{
namespace fuzzy
{

fuzzy_variable::fuzzy_variable()
    : m_min{ numeric_limits<float>::max() }
    , m_max{ numeric_limits<float>::min() }
{
}

fuzzy_variable::fuzzy_variable(fuzzy_variable&& rhs)
    : m_sets{ move(rhs.m_sets) }
    , m_min{ rhs.m_min }
    , m_max{ rhs.m_max }
{
}

fuzzy_variable::~fuzzy_variable() = default;

fuzzy_variable& fuzzy_variable::operator =(fuzzy_variable rhs)
{
    swap(rhs);
    return *this;
}

void fuzzy_variable::add_traiangle_set(float min, float mid, float max)
{
    m_sets.emplace_back(new fuzzy_set_triangle(min, mid, max));
    update_range(min, max);
}

void fuzzy_variable::add_trapezoid_set(float m1, float m2, float m3, float m4)
{
    m_sets.emplace_back(new fuzzy_set_trapezoid(m1, m2, m3, m4));
    update_range(m1, m4);
}

void fuzzy_variable::add_left_trapezoid_set(float min, float mid, float max)
{
    m_sets.emplace_back(new fuzzy_set_left_trapezoid(min, mid, max));
    update_range(min, max);
}

void fuzzy_variable::add_right_trapezoid_set(float min, float mid, float max)
{
    m_sets.emplace_back(new fuzzy_set_right_trapezoid(min, mid, max));
    update_range(min, max);
}

void fuzzy_variable::add_singleton_set(float m)
{
    m_sets.emplace_back(new fuzzy_set_singleton(m));
    update_range(m, m);
}

void fuzzy_variable::reset_dom()
{
    for_each(m_sets.begin(), m_sets.end(), [](fuzzy_set_ptr const& s) { s->reset_dom(); });
}

// Fuzzify input value
void fuzzy_variable::fuzzify(float input)
{
    for_each(m_sets.begin(), m_sets.end(), [input](fuzzy_set_ptr const& s)
    {
        s->set_dom(s->get_dom(input));
    });
}

void fuzzy_variable::update_range(float min, float max)
{
    m_min = std::min(min, m_min);
    m_max = std::max(max, m_max);
}

void fuzzy_variable::swap(fuzzy_variable& rhs)
{
    using std::swap;

    swap(m_sets, rhs.m_sets);
    swap(m_min, rhs.m_min);
    swap(m_max, rhs.m_max);
}

float fuzzy_variable::defuzzify_mean_max() const
{
    float total_val = 0.0f;
    float total_dom = 0.0f;

    for_each(m_sets.begin(), m_sets.end(), [&](fuzzy_set_ptr const& s)
    {
        total_val += s->get_dom() * s->get_mean_max();
        total_dom += s->get_dom();
    });

    return !math::is_zero(total_dom) ? total_val / total_dom : 0.0f;
}

float fuzzy_variable::defuzzify_centroid(std::size_t sample_count) const
{
    KISMET_ASSERT(sample_count > 0);

    float delta = (m_max - m_min) / sample_count;
    float input = m_min + delta;
    float total_dom = 0.0f;
    float total_value = 0.0f;

    while (sample_count--)
    {
        float max_dom = numeric_limits<float>::min();
        for (auto& s : m_sets)
        {
            max_dom = max(max_dom, min(s->get_dom(input), s->get_dom()));
        }

        total_dom += max_dom;
        total_value += max_dom * input;

        input += delta;
    }

    return !math::is_zero(total_dom) ? total_value / total_dom : 0.0f;
}

} // namespace fuzzy
} // namespace kismet
