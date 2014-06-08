#include <algorithm>
#include <limits>
#include "kismet/ai/fuzzy/fuzzy_or.h"
#include "kismet/utility.h"
using namespace std;

namespace kismet
{
namespace fuzzy
{

float fuzzy_or::get_dom() const
{
    float max_dom = numeric_limits<float>::min();
    for_each([&max_dom](fuzzy_term& t)
    {
        max_dom = max(t.get_dom(), max_dom);
    });

    return max_dom == numeric_limits<float>::min() ? 0.0f : max_dom;
}

unique_ptr<fuzzy_term> fuzzy_or::clone() const
{
    return make_unique<fuzzy_or>(*this);
}

} // namespace fuzzy
} // namespace kismet
