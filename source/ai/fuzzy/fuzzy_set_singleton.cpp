#include "kismet/ai/fuzzy/fuzzy_set_singleton.h"

namespace kismet
{
namespace fuzzy
{

fuzzy_set_singleton::fuzzy_set_singleton(float m)
    : fuzzy_set(m)
{
}

float fuzzy_set_singleton::do_get_dom(float input) const
{
    return input == get_mean_max() ? 1.0f : 0.0f;
}

} // namespace fuzzy
} // namespace kismet
