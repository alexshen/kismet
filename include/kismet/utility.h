#ifndef KISMET_UTILITY_UTILITY_H
#define KISMET_UTILITY_UTILITY_H

#include "kismet/config.h"
#include "kismet/core/assert.h"
#include <algorithm>
#include <iterator>
#include <memory>
#include <type_traits>

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

template<typename It>
using iterator_value_t = typename std::iterator_traits<It>::value_type;

namespace detail
{
template<typename It, typename OutIt>
inline OutIt checked_copy(It beg, It end,
                         std::size_t N, OutIt dest, 
                         iterator_value_t<OutIt> val,
                         std::input_iterator_tag)
{
    std::size_t size = 0;
    while (beg != end)
    {
        KISMET_ASSERT(size < N);
        *dest++ = *beg++;
    }
    return std::fill_n(dest, N - size, val);
}

template<typename It, typename OutIt>
inline OutIt checked_copy(It beg, It end,
                         std::size_t N, OutIt dest, 
                         iterator_value_t<OutIt> val,
                         std::random_access_iterator_tag)
{
    auto size = std::distance(beg, end);
    KISMET_ASSERT(size >= 0 && static_cast<std::size_t>(size) <= N);
    dest = std::copy(beg, end, dest);
    return std::fill_n(dest, N - size, val);
}
}

/// Copy [beg, end) to OutIt
/// The size of the source range must be <= N
/// If size is < N, then N - size elements with val will be output to OutIt
template<typename It, typename OutIt>
inline OutIt checked_copy(It beg, It end,
                         std::size_t N, OutIt dest, 
                         iterator_value_t<OutIt> val)
{
    using iterator_category = typename std::iterator_traits<It>::iterator_category;
    return detail::checked_copy(beg, end, N, dest, val, iterator_category());
}

template<bool value>
using boolean_constant_t = std::integral_constant<bool, value>;

} // namespace kismet

#endif // KISMET_UTILITY_UTILITY_H
