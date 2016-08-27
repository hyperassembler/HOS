#ifndef _BIFROST_ASSERT_H
#define _BIFROST_ASSERT_H

#include "bifrost_types.h"

void hw_assert_ex(const char* expr_str, const char* file, int32_t line, int32_t expr);

#define hw_assert(expr) hw_assert_ex(#expr, __FILE__, __LINE__, expr)

#endif
