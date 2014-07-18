#include "kismet/config.h"
// workaround mingw32 gcc
#ifdef KISMET_MINGW32
extern "C" void putenv(char*);
#endif

#define BOOST_TEST_MODULE kismet
#include <boost/test/included/unit_test.hpp>
