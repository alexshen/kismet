#include "kismet/config.h"
// workaround mingw32 gcc
#ifdef KISMET_MINGW32
extern "C" void putenv(char*);
#endif

#define BOOST_TEST_MODULE kismet
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
