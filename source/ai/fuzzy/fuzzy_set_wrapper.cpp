#include <algorithm>
#include <memory>

#include "kismet/ai/fuzzy/fuzzy_set_wrapper.h"
#include "kismet/ai/fuzzy/fuzzy_set.h"
#include "kismet/utility.h"

namespace kismet
{
namespace fuzzy
{

fuzzy_set_wrapper::fuzzy_set_wrapper(fuzzy_set& s)
    : m_set(s)
{
}

float fuzzy_set_wrapper::get_dom() const
{
    return m_set.get_dom();
}

void fuzzy_set_wrapper::aggregate(float dom)
{
    m_set.set_dom(std::max(m_set.get_dom(), dom));
}

std::unique_ptr<fuzzy_term> fuzzy_set_wrapper::clone() const
{
    return std::make_unique<fuzzy_set_wrapper>(*this);
}
} // namespace fuzzy
} // namespace kismet
