#ifndef _KERNEL_KE_ASSERT_H_
#define _KERNEL_KE_ASSERT_H_

#include "type.h"

void KABI ke_assert_ex(const char* expr_str, const char* file, int32_t line, int32_t expr);

#define ke_assert(expr) ke_assert_ex(#expr, __FILE__, __LINE__, expr)

#endif