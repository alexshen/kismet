#ifndef KISMET_COMMON_TYPE_H
#define KISMET_COMMON_TYPE_H

#include <type_traits>

namespace kismet
{
template<typename T, typename U>
using common_type_t = typename std::common_type<T, U>::type;

} // namespace kismet

#endif // KISMET_COMMON_TYPE_H
