#ifndef KISMET_FUZZY_SET_WRAPPER_H
#define KISMET_FUZZY_SET_WRAPPER_H

#include "kismet/ai/fuzzy/fuzzy_term.h"

namespace kismet
{
namespace fuzzy
{

class fuzzy_set;

class fuzzy_set_wrapper : public fuzzy_term
{
public:
    fuzzy_set_wrapper(fuzzy_set& s);

    float get_dom() const override;

    void aggregate(float dom) override;

    std::unique_ptr<fuzzy_term> clone() const override;
private:
    fuzzy_set& m_set;
};

} // namespace fuzzy
} // namespace kismet

#endif // KISMET_FUZZY_SET_WRAPPER_H
