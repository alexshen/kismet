#include <algorithm>
#include <limits>
#include "kismet/ai/fuzzy/fuzzy_and.h"
#include "kismet/utility.h"

namespace kismet
{
namespace fuzzy
{

float fuzzy_and::get_dom() const
{
    float min_dom = std::numeric_limits<float>::max();
    for_each([&min_dom](fuzzy_term& t)
    {
        min_dom = std::min(t.get_dom(), min_dom);
    });

    return min_dom == std::numeric_limits<float>::max() ? 0.0f : min_dom;
}

std::unique_ptr<fuzzy_term> fuzzy_and::clone() const
{
    return make_unique<fuzzy_and>(*this);
}

} // namespace fuzzy
} // namespace kismet
