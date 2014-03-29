#ifndef KISMET_CORE_ASSERT_H
#define KISMET_CORE_ASSERT_H

#include <cstdint>
#include <cassert>

#define KISMET_ASSERT assert
#define KISMET_ASSERT_ALIGN(p, a) KISMET_ASSERT((reinterpret_cast<std::uintptr_t>(p) & ~a) == 0)

#endif // KISMET_CORE_ASSERT_H

