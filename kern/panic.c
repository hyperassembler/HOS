#include <kern/panic.h>
#include <kern/print.h>
#include <kern/cdef.h>

#include <arch/cpu.h>

void panic(uint32 reason)
{
    kprintf("BugCheck: Reason - %ul\n", reason);
    arch_halt();
}
