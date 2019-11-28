#pragma once

#include <kern/cdef.h>
#include <kern/print.h>
#include <arch/brute.h>

#define BRUTE(fmt, ...) do { \
    kprintf("Kernel brute: " fmt "\n", ##__VA_ARGS__); \
    arch_brute(); \
} while(0)

#define KASSERT(expr, msg, ...) do { \
    if (!(expr)) { \
        BRUTE("Assertion \"" #expr "\" failed at %s:%d: " msg , __FILE__, __LINE__ , ##__VA_ARGS__); \
    } \
} while(0)
