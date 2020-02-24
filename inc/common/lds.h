#pragma once

#include <common/cdef.h>

#define LDS_DECL(name) \
    extern const char __start_##name[]; \
    extern const char __stop_##name[]

#define LDS_START(name) ((void*)__start_##name)
#define LDS_STOP(name) ((void*)__stop_##name)
#define LDS_ATTR(name) ATTR_SECTION(name)
