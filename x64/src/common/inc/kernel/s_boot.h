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
    intr_info_t intr_info;
    pmm_info_t* pmm_info;
    char cpu_vd_str[13];
} boot_info_t;

extern void KABI ke_main(boot_info_t *info);

#endif