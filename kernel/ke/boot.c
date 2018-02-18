#include "kernel/ke/boot.h"
#include "kernel/ke/bug_check.h"

/**
 * Kernel entry point
 * @param boot_info passed by the bootloader
 */
void KABI ke_main(boot_info_t *boot_info)
{
    status_t status = STATUS_SUCCESS;
    status = hal_init(boot_info);
    if (!sx_success(status))
    {
	    ke_panic(status);
        return;
    }

	ke_trap();
}
