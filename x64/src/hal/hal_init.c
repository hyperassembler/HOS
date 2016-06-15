/* Copyright 2016 secXsQuared
 * Distributed under GPL license
 * See COPYING under root for details
 */

#include "bit_ops.h"
#include "hal_print.h"
#include "hal_mem.h"
#include "hal_intr.h"
#include "hal_var.h"
#include "std_lib.h"
#include "s_boot.h"

static void KAPI _hal_obtain_cpu_info(k_boot_info_t *hal_info)
{
    if(hal_info == NULL)
        return;
    uint32_t eax = 0, ebx = 0, ecx = 0, edx = 0;
    hal_cpuid(&eax,&ebx,&ecx,&edx);
    mem_cpy(&ebx, &hal_info->cpu_vd_str[0], sizeof(uint32_t));
    mem_cpy(&edx, &hal_info->cpu_vd_str[4], sizeof(uint32_t));
    mem_cpy(&ecx, &hal_info->cpu_vd_str[8], sizeof(uint32_t));
    hal_info->cpu_vd_str[12] = 0;
}

static void KAPI _hal_init_gdt()
{

    // get gdt ready
    hal_write_segment_descriptor((void *) &g_gdt[0], 0, 0, 0);
    hal_write_segment_descriptor((void *) &g_gdt[8], 0, 0,
                                 SEG_DPL_0 | SEG_CODE_DATA | SEG_PRESENT | SEG_LONG | SEG_TYPE_CODE_X);
    hal_write_segment_descriptor((void *) &g_gdt[16], 0, 0,
                                 SEG_DPL_0 | SEG_CODE_DATA | SEG_PRESENT | SEG_LONG | SEG_TYPE_DATA_RW);
    hal_write_segment_descriptor((void *) &g_gdt[24], 0, 0,
                                 SEG_DPL_3 | SEG_CODE_DATA | SEG_PRESENT | SEG_LONG | SEG_TYPE_CODE_X);
    hal_write_segment_descriptor((void *) &g_gdt[32], 0, 0,
                                 SEG_DPL_3 | SEG_CODE_DATA | SEG_PRESENT | SEG_LONG | SEG_TYPE_DATA_RW);

    hal_write_segment_descriptor((void *) &g_gdt[40], 0, 0xFFFFF,
                                 SEG_DPL_0 | SEG_GRANULARITY | SEG_CODE_DATA | SEG_PRESENT | SEG_32_BITS |
                                 SEG_TYPE_CODE_X);
    hal_write_segment_descriptor((void *) &g_gdt[48], 0, 0xFFFFF,
                                 SEG_DPL_0 | SEG_GRANULARITY | SEG_CODE_DATA | SEG_PRESENT | SEG_32_BITS |
                                 SEG_TYPE_DATA_RW);
    hal_write_segment_descriptor((void *) &g_gdt[56], 0, 0xFFFFF,
                                 SEG_DPL_3 | SEG_GRANULARITY | SEG_CODE_DATA | SEG_PRESENT | SEG_32_BITS |
                                 SEG_TYPE_CODE_X);
    hal_write_segment_descriptor((void *) &g_gdt[64], 0, 0xFFFFF,
                                 SEG_DPL_3 | SEG_GRANULARITY | SEG_CODE_DATA | SEG_PRESENT | SEG_32_BITS |
                                 SEG_TYPE_DATA_RW);
    g_gdt_ptr.base = (uint64_t) g_gdt;
    g_gdt_ptr.limit = GDT_ENTRY_NUM * GDT_ENTRY_SIZE - 1;
    hal_flush_gdt(&g_gdt_ptr, seg_selector(1, 0), seg_selector(2, 0));
};

int32_t KAPI hal_init(void *m_info, k_boot_info_t* boot_info)
{
    if (m_info == NULL || boot_info == NULL || (uint64_t) m_info & bit_field_mask_64(0, 2))
        return 1;

    text_pos = get_pos(0, 0);

    // set up GDT
    _hal_init_gdt();

    // set up HAL heap;
    hal_alloc_init();

    // set up HAL def
    boot_info->krnl_start = (uint64_t)kernel_start;
    boot_info->krnl_end = (uint64_t)kernel_end;

    // obtain cpu info
    _hal_obtain_cpu_info(boot_info);

    // init interrupt
    if(hal_interrupt_init() != 0)
    {
        return 1;
    }

    return 0;
}
