#pragma once

#include <common/cdef.h>
#include <ke/print.h>
#include <arch/brute.h>

#define BRUTE(fmt, ...) do { \
    kprintf("Kernel brute at %s:%d: " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__); \
    arch_brute(); \
} while(0)

#define KASSERT(expr, msg, ...) do { \
    if (!(expr)) { \
        BRUTE("Assertion \"" #expr "\" failed: " msg , ##__VA_ARGS__); \
    } \
} while(0)
