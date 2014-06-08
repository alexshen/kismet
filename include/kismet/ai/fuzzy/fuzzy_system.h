#ifndef KISMET_FUZZY_SYSTEM_H
#define KISMET_FUZZY_SYSTEM_H

#include <cstddef>
#include <string>
#include <vector>
#include <unordered_map>

#include "kismet/ai/fuzzy/fuzzy_variable.h"
#include "kismet/ai/fuzzy/fuzzy_rule.h"
#include "kismet/ai/fuzzy/fuzzy_term.h"

namespace kismet
{
namespace fuzzy
{

class fuzzy_variable;

/**
 * A fuzzy system which manages fuzzy variables and fuzzy rules
 * is used for fuzzy inference.
 * Mamdani type of fuzzy inference is implemented.
 */
class fuzzy_system
{
public:
    using fuzzy_id = std::string;

    fuzzy_system() = default;

    fuzzy_system(fuzzy_system const&) = delete;
    fuzzy_system& operator =(fuzzy_system const&) = delete;

    /**
     * Add a new variable to the system.
     */
    fuzzy_variable& add_variable(fuzzy_id const& id);
    fuzzy_variable& get_variable(fuzzy_id const& id);

    /**
     * Check if the system has a variable
     */
    bool has_variable(fuzzy_id const& id) const;

    void add_rule(fuzzy_term_ptr antecedent, fuzzy_term_ptr consequent);

    /**
     * Fuzzify input of the specified variable
     */
    void fuzzify(fuzzy_id const& var_id, float input);

    /**
     * Defuzzify using the mean max method
     */
    float defuzzify_mean_max(fuzzy_id const& id);

    /**
     * Defuzzify using centroid method
     */
    float defuzzify_centroid(fuzzy_id const& id, std::size_t sample_count);
private:
    using variable_map = std::unordered_map<fuzzy_id, fuzzy_variable>;
    using rule_base    = std::vector<fuzzy_rule>;

    variable_map m_vars;
    rule_base    m_rules;
};

} // namespace fuzzy
} // namespace kismet

#endif // KISMET_FUZZY_SYSTEM_H
