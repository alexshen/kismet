#include <utility>
#include <algorithm>
#include "kismet/ai/fuzzy/fuzzy_rule.h"
#include "kismet/core/assert.h"
using namespace std;

namespace kismet
{
namespace fuzzy
{

fuzzy_rule::fuzzy_rule(fuzzy_term_ptr antecedent, fuzzy_term_ptr consequent)
    : m_antecedent{ move(antecedent) }
    , m_consequent{ move(consequent) }
{
    KISMET_ASSERT(m_antecedent && m_consequent);
}

fuzzy_rule::fuzzy_rule(fuzzy_rule&& rhs)
    : m_antecedent{ move(rhs.m_antecedent) }
    , m_consequent{ move(rhs.m_consequent) }
{
}

fuzzy_rule::~fuzzy_rule() = default;

fuzzy_rule& fuzzy_rule::operator =(fuzzy_rule rhs)
{
    swap(rhs);
    return *this;
}

void fuzzy_rule::swap(fuzzy_rule& rhs)
{
    using std::swap;

    swap(m_antecedent, rhs.m_antecedent);
    swap(m_consequent, rhs.m_consequent);
}

} // namespace fuzzy
} // namespace kismet
