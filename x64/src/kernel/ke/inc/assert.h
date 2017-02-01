#ifndef _ASSERT_H_
#define _ASSERT_H_

#include "g_type.h"
#include "g_abi.h"

void KABI ke_assert_ex(const char* expr_str, const char* file, int32_t line, int32_t expr);

#define ke_assert(expr) ke_assert_ex(#expr, __FILE__, __LINE__, expr)

#endif