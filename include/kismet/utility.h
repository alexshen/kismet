#ifndef KISMET_UTILITY_UTILITY_H
#define KISMET_UTILITY_UTILITY_H

#include "kismet/config.h"
#include <memory>

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

} // namespace kismet

#endif // KISMET_UTILITY_UTILITY_H
