#include "assert.h"
#include "print.h"

void ke_assert_ex(const char* expr_str, const char* file, int32_t line, int32_t expr)
{
    if(!expr)
    {
        ke_printf("Assertion \"%s\" failed at %s:%d.\n", expr_str, file, line);
    }
}