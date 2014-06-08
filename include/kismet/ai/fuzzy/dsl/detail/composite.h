#ifndef KISMET_FUZZY_DSL_COMPOSITE_H
#define KISMET_FUZZY_DSL_COMPOSITE_H

#include <cstddef>
#include <tuple>
#include <memory>
#include "kismet/ai/fuzzy/dsl/term.h"
#include "kismet/utility.h"

namespace kismet
{
namespace fuzzy
{
namespace dsl
{

namespace detail
{

template<typename T, typename U, std::size_t I, std::size_t N>
struct composite_get_term_helper
{
    static void setup(T& term, U& t)
    {
        term.add(get_term(std::get<I>(t)));
        using helper_type = composite_get_term_helper<T, U, I+1, N>;
        helper_type::setup(term, t);
    }
};

template<typename T, typename U, std::size_t I>
struct composite_get_term_helper<T, U, I, I>
{
    static void setup(T& term, U& t) {}
};

template<typename T, typename U>
inline std::unique_ptr<T> composite_get_term(U& t)
{
    auto term = make_unique<T>();
    using helper_type = composite_get_term_helper<T, U, 0, std::tuple_size<U>::value>;
    helper_type::setup(*term, t);
    return term;
}

} // namespace detail

} // namespace dsl
} // namespace fuzzy
} // namespace kismet

#endif // KISMET_FUZZY_DSL_COMPOSITE_H 

