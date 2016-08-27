#include "bifrost_hs_boot.h"
#include "mem.h"


int32_t arch_mem_init(hw_arch_bootinfo_t* mem_info)
{
    mem_info->mem_count = 0;
    return 0;
}
