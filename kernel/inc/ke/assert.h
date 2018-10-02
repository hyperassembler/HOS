#pragma once

#include "cdef.h"

#define KE_ASSERT(expr) ke_assert_ex(#expr, __FILE__, __LINE__, expr)

void
ke_assert_ex(const char *expr_str, const char *file, int32 line, int32 expr);
