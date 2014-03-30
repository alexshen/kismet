#ifndef KISMET_IS_EITHER_CONVERTIBLE_H
#define KISMET_IS_EITHER_CONVERTIBLE_H

#include <type_traits>

namespace kismet
{

template<typename T, typename U>
struct is_either_convertible :
    std::integral_constant<bool,
        std::is_convertible<T, U>::value ||
        std::is_convertible<U, T>::value>
{};

} // namespace kismet

#endif // KISMET_IS_EITHER_CONVERTIBLE_H 

