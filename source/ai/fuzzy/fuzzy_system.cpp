#include <algorithm>
#include <utility>
#include "kismet/ai/fuzzy/fuzzy_system.h"
#include "kismet/core/assert.h"

using namespace std;

namespace kismet
{
namespace fuzzy
{

fuzzy_variable& fuzzy_system::add_variable(fuzzy_id const& id)
{
    KISMET_ASSERT(!id.empty() && !has_variable(id));

    auto it = m_vars.emplace(id, fuzzy_variable{});
    return (*it.first).second;
}

fuzzy_variable& fuzzy_system::get_variable(fuzzy_id const& id)
{
    KISMET_ASSERT(!id.empty() && has_variable(id));

    return m_vars.find(id)->second;
}

void fuzzy_system::add_rule(fuzzy_term_ptr antecedent, fuzzy_term_ptr consequent)
{
    m_rules.emplace_back(move(antecedent), move(consequent));
}

bool fuzzy_system::has_variable(fuzzy_id const& id) const
{
    return m_vars.find(id) != m_vars.end();
}

void fuzzy_system::fuzzify(fuzzy_id const& id, float input)
{
    KISMET_ASSERT(has_variable(id));

    get_variable(id).fuzzify(input);
}

float fuzzy_system::defuzzify_mean_max(fuzzy_id const& id)
{
    KISMET_ASSERT(has_variable(id));

    auto& var = get_variable(id);
    var.reset_dom();

    // run through all rules to calculate the consequent dom
    for_each(m_rules.begin(), m_rules.end(), [](fuzzy_rule& r) { r.calculate(); });

    return var.defuzzify_mean_max();
}

float fuzzy_system::defuzzify_centroid(fuzzy_id const& id, std::size_t sample_count)
{
    KISMET_ASSERT(has_variable(id) && sample_count > 0);

    auto& var = get_variable(id);
    var.reset_dom();

    // run through all rules to calculate the consequent dom
    for_each(m_rules.begin(), m_rules.end(), [](fuzzy_rule& r) { r.calculate(); });

    return var.defuzzify_centroid(sample_count);
}

} // namespace fuzzy
} // namespace kismet
