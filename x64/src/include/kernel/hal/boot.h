#ifndef _S_BOOT_H_
#define _S_BOOT_H_

#include "../../abi.h"
#include "../../../mm/arch/amd64/mm.h"
#include "intr.h"
#include "../../status.h"

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

extern status_t KABI hal_init (_IN  void* multiboot_info,
                               _OUT boot_info_t** boot_info);

#endif