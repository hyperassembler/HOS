#include "kernel/ke/boot.h"
#include "kernel/ke/bug_check.h"

/**
 * Kernel entry point
 * @param boot_info passed by the bootloader
 */
void SXAPI ke_main(struct boot_info *boot_info)
{
	sx_status status = STATUS_SUCCESS;
	status = hal_init(boot_info);
	if (!sx_success(status))
	{
		ke_panic(status);
	}
	ke_trap();
}

