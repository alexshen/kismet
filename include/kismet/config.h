#ifndef KISMET_CONFIG_H
#define KISMET_CONFIG_H

#if defined(_MSC_VER)
#  define KISMET_MSC _MSC_VER
#  define KISMET_NO_EXTERN_TEMPLATE
#elif defined(__GNUG__)
#  define KISMET_GCC __GNUG__
#  if defined(__MINGW32__)
#    define KISMET_MINGW32
#  endif
#else
#  error "Unknown compiler"
#endif

#endif // KISMET_CONFIG_H
