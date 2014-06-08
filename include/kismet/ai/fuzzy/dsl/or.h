#ifndef KISMET_FUZZY_DSL_OR_H
#define KISMET_FUZZY_DSL_OR_H

#include <tuple>
#include <utility>
#include <memory>
#include "kismet/ai/fuzzy/dsl/term.h"
#include "kismet/ai/fuzzy/dsl/detail/composite.h"
#include "kismet/ai/fuzzy/fuzzy_or.h"
#include "kismet/utility.h"

namespace kismet
{
namespace fuzzy
{
namespace dsl
{

template<typename... T>
struct fz_or : term<fz_or<T...>>
{
    fz_or(T... t)
        : terms{ t... }
    {
    }

    std::unique_ptr<fuzzy_term> get_term()
    {
        return detail::composite_get_term<fuzzy_or>(terms);
    }

    std::tuple<T...> terms;
};

template<typename... T>
inline typename std::enable_if<
                    all_tags_or_fuzzy_terms<T...>::value
                  , fz_or<T...>>::type
                or_(T&&... arg)
{
    static_assert(sizeof...(T) <= fuzzy_and::max_terms, "too many terms");
    return fz_or<T...>{ std::forward<T>(arg)... };
}

} // namespace dsl
} // namespace fuzzy
} // namespace kismet

#endif // KISMET_FUZZY_FUZZY_DSL_OR_H
