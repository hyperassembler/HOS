#pragma once

#include <kern/cdef.h>

/**
 * Specific error codes
 */
typedef enum {
    SUCCESS = 0x0,
    ENOMEM = 0x1,
    EINVARG = 0x2,
    EINIT = 0x3,
    EDUP = 0x4
} kstatus;

