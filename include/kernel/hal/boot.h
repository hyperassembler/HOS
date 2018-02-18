#ifndef _KERNEL_HAL_BOOT_H_
#define _KERNEL_HAL_BOOT_H_

#include "type.h"
#include "kernel/hal/intr.h"
#include "kernel/hal/mem.h"
#include "status.h"

/**
 * Required OS boot info
 */

typedef struct
{
	void* krnl_end;
    intr_info_t intr_info;
    char cpu_vd_str[13];
} boot_info_t;

status_t KABI hal_init(void *m_info);

#endif