#ifndef KISMET_FUZZY_DSL_TERM_H
#define KISMET_FUZZY_DSL_TERM_H

#include <type_traits>
#include <memory>
#include "kismet/ai/fuzzy/dsl/tag.h"
#include "kismet/ai/fuzzy/fuzzy_term.h"
#include "kismet/ai/fuzzy/fuzzy_set.h"
#include "kismet/ai/fuzzy/fuzzy_set_wrapper.h"
#include "kismet/enable_if_convertible.h"
#include "kismet/utility.h"

namespace kismet
{
namespace fuzzy
{

namespace dsl
{

/**
 * Base class for all dsl term
 */
template<typename T>
struct term : tag
{
    operator std::unique_ptr<fuzzy_term>()
    {
        return static_cast<T&>(*this).get_term();
    }
};

inline std::unique_ptr<fuzzy_term> get_term(fuzzy_set& s)
{
    return make_unique<fuzzy_set_wrapper>(s);
}

template<typename T>
inline enable_if_convertible_t<T, tag, std::unique_ptr<fuzzy_term>> get_term(T&& t)
{
    return t.get_term();
}

/**
 * Check if T is of tag or of fuzzy_term or of fuzzy_set
 */
template<typename T>
using is_tag_or_fuzzy_term = boolean_constant_t<
                                std::is_convertible<T, fuzzy_term const&>::value
                             || std::is_convertible<T, fuzzy_set const&>::value
                             || std::is_convertible<T, tag>::value>;

namespace detail
{
 
template<typename T, typename... U>
struct all_tags_or_fuzzy_terms_impl
{
    static const bool value = is_tag_or_fuzzy_term<T>::value && all_tags_or_fuzzy_terms_impl<U...>::value;
};

template<typename T>
struct all_tags_or_fuzzy_terms_impl<T>
{
    static const bool value = is_tag_or_fuzzy_term<T>::value;
};

}

/**
 * Check if all types are valid types
 */
template<typename... T>
struct all_tags_or_fuzzy_terms
{
    static const bool value = detail::all_tags_or_fuzzy_terms_impl<T...>::value;
};

template<>
struct all_tags_or_fuzzy_terms<>
{
    static const bool value = true;
};

} // namespace dsl
} // namespace fuzzy
} // namespace kismet

#endif // KISMET_FUZZY_DSL_TERM_H

