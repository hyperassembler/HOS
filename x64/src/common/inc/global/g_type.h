/* Copyright 2016 secXsQuared
 * Distributed under GPL license
 * See COPYING under root for details
 */

#ifndef _G_TYPE_H_
#define _G_TYPE_H_

#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stddef.h>

typedef int32_t (*callback_func_t)(void *kernel_args, void *user_args);

#define STRUCT_PACKED __attribute__((packed))

#define UNREFERENCED(x) {(x) = (x);}

#define _OUT
#define _IN
#define _IN_OUT
#define _IN_OPT
#define _OUT_OPT

#endif