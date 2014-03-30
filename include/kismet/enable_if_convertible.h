#ifndef KISMET_ENABLE_IF_CONVERTIBLE_H
#define KISMET_ENABLE_IF_CONVERTIBLE_H

#include <type_traits>

namespace kismet
{

template<typename From, typename To, typename T = void>
struct enable_if_convertible : std::enable_if<std::is_convertible<From, To>::value, T>
{};

template<typename From, typename To, typename T = void>
using enable_if_convertible_t = typename enable_if_convertible<From, To, T>::type;

} // namespace kimset

#endif // KISMET_ENABLE_IF_CONVERTIBLE_H

