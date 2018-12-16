#pragma once

#include <kern/cdef.h>

#define KASSERT(expr) kassert_ex(#expr, __FILE__, __LINE__, expr)

void
kassert_ex(const char *expr_str, const char *file, int32 line, int32 expr);
