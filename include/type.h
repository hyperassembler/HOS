#ifndef _TYPE_H_
#define _TYPE_H_

#define KABI __attribute__((sysv_abi))
#define UAPI __attribute__((sysv_abi))

#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stddef.h>

typedef int32_t (*callback_func_t)(void *kernel_args, void *user_args);

#define STRUCT_PACKED __attribute__((packed))

#define UNREFERENCED(x) {(x) = (x);}

#endif