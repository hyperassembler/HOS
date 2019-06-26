#include <kern/print.h>
#include <kern/assert.h>
#include <arch/print.h>

void
kprintf(const char *str, ...)
{
    va_list args;
    va_start(args, str);
    kvprintf(str, args);
    va_end(args);
}

void
kvprintf(const char *str, va_list args)
{
    UNREFERENCED(str);
    UNREFERENCED(args);
    arch_vprintf(str, args);
}
