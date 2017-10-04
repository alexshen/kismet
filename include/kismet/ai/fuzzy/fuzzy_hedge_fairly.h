#ifndef KISMET_FUZZY_HEDGE_FAIRLY_H
#define KISMET_FUZZY_HEDGE_FAIRLY_H

#include <utility>
#include <cmath>
#include "kismet/ai/fuzzy/fuzzy_term.h"
#include "kismet/core/assert.h"

namespace kismet
{
namespace fuzzy
{

class fuzzy_hedge_fairly : public fuzzy_term
{
public:
    fuzzy_hedge_fairly(fuzzy_term_ptr term)
        : m_term{ std::move(term) }
    {
        KISMET_ASSERT(m_term);
    }

    float get_dom() const override
    {
        float dom = m_term->get_dom();
        return dom * dom;
    }

    void aggregate(float dom) override
    {
        m_term->aggregate(std::sqrt(dom));
    }
private:
    fuzzy_term_ptr m_term;
};

} // namespace fuzzy
} // namespace kismet

#endif // KISMET_FUZZY_HEDGE_FAIRLY_H
