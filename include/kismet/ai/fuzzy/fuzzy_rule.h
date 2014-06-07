#ifndef KISET_AI_FUZZY_RULE_H
#define KISET_AI_FUZZY_RULE_H

#include <memory>
#include "kismet/ai/fuzzy/fuzzy_term.h"

namespace kismet
{
namespace fuzzy
{

class fuzzy_rule
{
public:
    fuzzy_rule(fuzzy_term_ptr antecedent, fuzzy_term_ptr consequent);
    ~fuzzy_rule();

    fuzzy_rule(fuzzy_rule&& rhs);
    fuzzy_rule& operator =(fuzzy_rule rhs);

    /**
     * Calculate dom of the consequent of the rule
     */
    void calculate()
    {
        m_consequent->aggregate(m_antecedent->get_dom());
    }

    void swap(fuzzy_rule& rhs);
private:
    fuzzy_term_ptr m_antecedent;
    fuzzy_term_ptr m_consequent;
};

inline void swap(fuzzy_rule& lhs, fuzzy_rule& rhs)
{
    lhs.swap(rhs);
}

} // namespace fuzzy
} // namespace kismet

#endif // !KISET_AI_FUZZY_RULE_H
