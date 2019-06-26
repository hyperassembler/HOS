#include <kern/cdef.h>
#include <kern/status.h>

#include <arch/cpu.h>
/**
 * Kernel entry point
 * @param boot_info passed by the bootloader
 */
void KABI
kmain(void *boot_info)
{
    UNREFERENCED(boot_info);
    arch_halt();
}

