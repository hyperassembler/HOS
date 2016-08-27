#ifndef _S_BOOT_H_
#define _S_BOOT_H_

#include "g_abi.h"
#include "s_pmm.h"
#include "s_intr.h"

//
// HAL Boot Info
//
typedef struct
{
    uint64_t krnl_start;
    uint64_t krnl_end;
    k_hal_intr_info_t intr_info;
    linked_list_t pmm_info;
    char cpu_vd_str[13];
} k_hal_boot_info_t;

extern void KAPI ke_main(k_hal_boot_info_t *info);

#endif