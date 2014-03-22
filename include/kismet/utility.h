#ifndef KISMET_UTILITY_UTILITY_H
#define KISMET_UTILITY_UTILITY_H

#include "kismet/config.h"

#ifndef KISMET_INSTANTIATE_TEMPLATE
#  ifndef KISMET_NO_EXTERN_TEMPLATE
#    define KISMET_CLASS_TEMPLATE_API(T, ...)   extern template class T<__VA_ARGS__>;
#    define KISMET_FUNC_TEMPLATE_API(F, R, ...) extern template R F(__VA_ARGS__);
#  else
#    define KISMET_CLASS_TEMPLATE_API(T, ...)
#    define KISMET_FUNC_TEMPLATE_API(F, R, ...)
#  endif
#else
#  define KISMET_CLASS_TEMPLATE_API(T, ...)   template class T<__VA_ARGS__>;
#  define KISMET_FUNC_TEMPLATE_API(F, R, ...) template R F(__VA_ARGS__);
#endif

#endif // KISMET_UTILITY_UTILITY_H
