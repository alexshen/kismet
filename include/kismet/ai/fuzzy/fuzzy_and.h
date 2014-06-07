#ifndef KISMET_FUZZY_AND_H
#define KISMET_FUZZY_AND_H

#include "kismet/ai/fuzzy/detail/fuzzy_composite.h"

namespace kismet
{
namespace fuzzy
{

class fuzzy_and : public detail::fuzzy_composite
{
public:
    float get_dom() const override;

    std::unique_ptr<fuzzy_term> clone() const override;
};

} // namespace fuzzy
} // namespace kismet

#endif // KISMET_FUZZY_AND_H
