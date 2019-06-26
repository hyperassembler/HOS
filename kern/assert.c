#include <kern/cdef.h>
#include <kern/assert.h>
#include <kern/print.h>

void kassert_ex(const char *expr_str, const char *file, int32 line, int32 expr)
{
    if (!expr)
    {
        kprintf("Assertion \"%s\" failed at %s:%d.\n", expr_str, file, line);
    }
}
