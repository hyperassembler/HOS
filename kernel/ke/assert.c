#include "type.h"
#include "kernel/ke/assert.h"
#include "kernel/ke/print.h"

void ke_assert_ex(const char *expr_str, const char *file, int32 line, int32 expr)
{
	if (!expr)
	{
		ke_printf("Assertion \"%s\" failed at %s:%d.\n", expr_str, file, line);
	}
}
