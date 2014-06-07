#include <utility>
#include "kismet/ai/fuzzy/detail/fuzzy_composite.h"
#include "kismet/core/assert.h"
using namespace std;

namespace kismet
{
namespace fuzzy
{
namespace detail
{

fuzzy_composite::fuzzy_composite(fuzzy_composite const& rhs)
{
    for (int i = 0; i < max_terms; ++i)
    {
        if (!rhs.m_terms[i])
        {
            break;
        }

        m_terms[i] = move(rhs.m_terms[i]->clone());
    }
}

void fuzzy_composite::aggregate(float dom)
{
    for_each([dom](fuzzy_term& t) { t.aggregate(dom); });
}

std::size_t fuzzy_composite::size() const
{
    std::size_t c = 0;
    for_each([&c](fuzzy_term& t) { ++c; });
    return c;
}

void fuzzy_composite::add(std::unique_ptr<fuzzy_term> term)
{
    auto count = size();
    KISMET_ASSERT(count < max_terms);

    m_terms[count] = move(term);
}

} // namespace detail
} // namespace fuzzy
} // namespace kismet
