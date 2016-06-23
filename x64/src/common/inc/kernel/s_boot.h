#ifndef _S_BOOT_H_
#define _S_BOOT_H_
#include "s_def.h"
//
// HAL Boot Info
//
typedef struct
{
    uint64_t krnl_start;
    uint64_t krnl_end;
    k_hal_intr_info_t intr_info;
    char cpu_vd_str[13];
} k_hal_boot_info_t;

#endif