#ifndef KERNEL_HAL_BOOT_H
#define KERNEL_HAL_BOOT_H

#include "type.h"
#include "kernel/hal/intr.h"
#include "kernel/hal/mem.h"
#include "status.h"

/**
 * Required OS boot info
 */

struct boot_info
{
	void *krnl_end;
	struct intr_info intr_info;
	char cpu_vd_str[13];
};

sx_status SXAPI hal_init(void *m_info);

#endif
