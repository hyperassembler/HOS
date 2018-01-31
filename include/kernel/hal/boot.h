#ifndef _KERNEL_HAL_BOOT_H_
#define _KERNEL_HAL_BOOT_H_

#include "type.h"
#include "kernel/hal/intr.h"
#include "kernel/hal/mem.h"

/**
 * Required OS boot info
 */

typedef struct
{
    virtual_addr_t krnl_start;
    virtual_addr_t krnl_end;
    intr_info_t intr_info;
    char cpu_vd_str[13];
} boot_info_t;

void KABI ke_main(boot_info_t* info);

#endif