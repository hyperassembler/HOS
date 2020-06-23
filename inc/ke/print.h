#pragma once

#include <common/cdef.h>
#include <ke/print.h>

ATTR_FMT_PRINTF int
kprintf(const char *fmt, ...);

int
kvprintf(const char *fmt, va_list args);

#ifdef KOPT_DBG
#define PDBG(fmt, ...) (kprintf("[DEBUG] " fmt "\n", ##__VA_ARGS__))
#else
#define PDBG(fmt, ...)
#endif

#define PWARN(fmt, ...) (kprintf("[WARN] " fmt "\n", ##__VA_ARGS__))
#define PINFO(fmt, ...) (kprintf("[INFO] " fmt "\n", ##__VA_ARGS__))
#define PERR(fmt, ...) (kprintf("[ERROR] " fmt "\n", ##__VA_ARGS__))
