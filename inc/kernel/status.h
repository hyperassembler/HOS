#pragma once

#include "common.h"

typedef uint32 k_status;

/**
 * Specific error codes
 */

#define STATUS_SUCCESS        (0x0)

#define STATUS_INVALID_ARGS   (0x1)
#define STATUS_NO_MEM         (0x2)
#define STATUS_UNINITIALIZED  (0x3)
#define STATUS_DUPLICATE      (0x4)

#define SX_SUCCESS(val) ((val) == (STATUS_SUCCESS))
