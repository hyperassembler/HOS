#ifndef _BIFROST_TYPES_H_
#define _BIFROST_TYPES_H_

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdarg.h>

typedef void (*hw_callback_func_t)(void* kp, void* up);

typedef uint32_t hw_handle_t;

#define TRUE  (true)
#define FALSE (false)

#endif
