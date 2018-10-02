#include "kernel.h"
#include "cdef.h"
#include "intrp.h"
#include "mm/pmm.h"
#include "ke/panic.h"

/**
 * Kernel entry point
 * @param boot_info passed by the bootloader
 */
void KABI
kmain(struct boot_info *boot_info)
{
    k_status status;

    // initialize interrupts
    status = kp_intr_init(boot_info);
    if (!SX_SUCCESS(status))
    {
        goto end;
    }

    // initialize memory manager
    status = mm_pmm_init(boot_info);
    if (!SX_SUCCESS(status))
    {
        goto end;
    }

end:
    if (!SX_SUCCESS(status))
    {
        ke_panic(status);
    }

    ke_panic(0x0);
}

