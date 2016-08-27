#ifndef _BIFROST_HS_BOOT_H_
#define _BIFROST_HS_BOOT_H_

#include "stdint.h"
#include "bifrost_hs_intr.h"
#include "bifrost_hs_mem.h"

typedef struct {
    hw_arch_intr_info_t int_info;
    uint32_t mem_count;
    hw_arch_memory_info_t mem_info[];
} hw_arch_bootinfo_t;

extern int32_t ke_hal_setup(hw_arch_bootinfo_t *bootinfo);

#endif
