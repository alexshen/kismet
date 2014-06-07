#ifndef KISMET_FUZZY_SET_SINGLETON_H
#define KISMET_FUZZY_SET_SINGLETON_H

#include "kismet/ai/fuzzy/fuzzy_set.h"

namespace kismet
{
namespace fuzzy
{

class fuzzy_set_singleton : public fuzzy_set
{
public:
    fuzzy_set_singleton(float m);
private:
    float do_get_dom(float input) const override;
};

} // namespace fuzzy
} // namespace kismet

#endif // KISMET_FUZZY_SET_SINGLETON_H
