#ifndef _K_ASSERT_H_
#define _K_ASSERT_H_

#include "g_type.h"

void hw_assert_ex(const char* expr_str, const char* file, int32_t line, int32_t expr);

#define hw_assert(expr) hw_assert_ex(#expr, __FILE__, __LINE__, expr)

#endif