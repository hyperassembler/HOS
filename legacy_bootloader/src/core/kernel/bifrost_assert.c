#include "bifrost_assert.h"
#include "bifrost_print.h"

void hw_assert_ex(const char* expr_str, const char* file, int32_t line, int32_t expr)
{
    if (!expr)
    {
        hw_printf("Assertion \"%s\" failed at %s: %d\n", expr_str, file, line);
    }
    return;
}
