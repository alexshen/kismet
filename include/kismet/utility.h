#ifndef KISMET_UTILITY_UTILITY_H
#define KISMET_UTILITY_UTILITY_H

#include "kismet/config.h"
#include "kismet/core/assert.h"
#include <memory>
#include <algorithm>

#ifndef KISMET_INSTANTIATE_TEMPLATE
#  ifndef KISMET_NO_EXTERN_TEMPLATE
#    define KISMET_CLASS_TEMPLATE_API(CLS_TEMPLATE_NAME_, ...) \
        extern template class CLS_TEMPLATE_NAME_<__VA_ARGS__>;
#    define KISMET_FUNC_TEMPLATE_API(FUNC_TEMPLATE_NAME_, FUNC_TEMPLATE_RETURN_TYPE_, ...) \
        extern template FUNC_TEMPLATE_RETURN_TYPE_ FUNC_TEMPLATE_NAME_(__VA_ARGS__);
#  else
#    define KISMET_CLASS_TEMPLATE_API(CLS_TEMPLATE_NAME_, ...)
#    define KISMET_FUNC_TEMPLATE_API(FUNC_TEMPLATE_NAME_, FUNC_TEMPLATE_RETURN_TYPE_, ...)
#  endif
#else
#  define KISMET_CLASS_TEMPLATE_API(CLS_TEMPLATE_NAME_, ...) \
        template class CLS_TEMPLATE_NAME_<__VA_ARGS__>;
#  define KISMET_FUNC_TEMPLATE_API(FUNC_TEMPLATE_NAME_, FUNC_TEMPLATE_RETURN_TYPE_, ...) \
        template FUNC_TEMPLATE_RETURN_TYPE_ FUNC_TEMPLATE_NAME_(__VA_ARGS__);
#endif

namespace kismet
{

template<typename T, typename... Args>
inline std::unique_ptr<T> make_unique(Args&&... args)
{
    return std::unique_ptr<T>(new T{ std::forward<Args>(args)... });
}

/// In-place reorder a range based on the given indices, indices will be modified.
/// Index cannot be duplicate, otherwise behavior is undefined.
template<typename RandIt, typename IndexRandIt>
void reorder(RandIt start, RandIt end, IndexRandIt index_start)
{
    using std::iter_swap;

    using difference_type = typename std::iterator_traits<RandIt>::difference_type;
    auto dist = end - start;
    KISMET_ASSERT(dist >= 0);

    auto index_end = index_start + dist;
    for (difference_type i = 0; i < dist; ++i, ++index_start)
    {
        // if current item's index is not i
        if (*index_start != i)
        {
            // bring the target item into this place
            iter_swap(start + i, start + *index_start);

            // as current item has been move to index_start[i]
            // we need to update its corresponding index so that
            // it can be correctly found
            auto it = std::find(index_start + 1, index_end, i);
            KISMET_ASSERT(it != index_end);
            *it = *index_start;
        }
    }
}

} // namespace kismet

#endif // KISMET_UTILITY_UTILITY_H
