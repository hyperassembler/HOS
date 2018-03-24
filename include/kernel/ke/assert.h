#ifndef KERNEL_KE_ASSERT_H
#define KERNEL_KE_ASSERT_H

#include "type.h"

void SXAPI ke_assert_ex(const char *expr_str, const char *file, int32 line, int32 expr);

#define ke_assert(expr) ke_assert_ex(#expr, __FILE__, __LINE__, expr)

#endif
