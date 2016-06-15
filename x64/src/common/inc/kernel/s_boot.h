#ifndef _S_BOOT_H_
#define _S_BOOT_H_
#include "s_def.h"
//
// Boot Info
//
typedef struct
{
    uint32_t priority_low;
    uint32_t priority_high;
} k_boot_intr_info_t;

typedef struct
{
    uint64_t krnl_start;
    uint64_t krnl_end;
    k_boot_intr_info_t intr_info;
    char cpu_vd_str[13];
} k_boot_info_t;

#endif