/* Copyright 2016 secXsQuared
 * Distributed under GPL license
 * See COPYING under root for details
 */

#include "g_abi.h"
#include "g_type.h"
#include "hal_mem.h"
#include "k_salloc.h"
#include "hal_arch.h"
#include "hal_intr.h"

static uint8_t _gdts[HAL_CORE_COUNT][GDT_ENTRY_NUM * GDT_ENTRY_SIZE];
static hal_gdt_ptr_t _gdt_ptrs[HAL_CORE_COUNT];

#define KERNEL_HEAP_SIZE 8192

char kernel_heap[KERNEL_HEAP_SIZE];

void KAPI hal_write_pt_entry(void *const base, uint64_t const p_addr, uint64_t const attr)
{
    if (base == NULL)
        return;
    uint64_t entry = (p_addr & 0xFFFFFFFFFF000) | attr;
    ((uint8_t *) base)[0] = (uint8_t) (entry & 0xFF);
    ((uint8_t *) base)[1] = (uint8_t) ((entry >> 8) & 0xFF);
    ((uint8_t *) base)[2] = (uint8_t) ((entry >> 16) & 0xFF);
    ((uint8_t *) base)[3] = (uint8_t) ((entry >> 24) & 0xFF);
    ((uint8_t *) base)[4] = (uint8_t) ((entry >> 32) & 0xFF);
    ((uint8_t *) base)[5] = (uint8_t) ((entry >> 40) & 0xFF);
    ((uint8_t *) base)[6] = (uint8_t) ((entry >> 48) & 0xFF);
    ((uint8_t *) base)[7] = (uint8_t) ((entry >> 56) & 0xFF);
    return;
}

void KAPI hal_write_pd_entry(void *const base, uint64_t const pt_addr, uint64_t const attr)
{
    if (base == NULL)
        return;
    uint64_t entry = (pt_addr & 0xFFFFFFFFFF000) | attr;
    ((uint8_t *) base)[0] = (uint8_t) (entry & 0xFF);
    ((uint8_t *) base)[1] = (uint8_t) ((entry >> 8) & 0xFF);
    ((uint8_t *) base)[2] = (uint8_t) ((entry >> 16) & 0xFF);
    ((uint8_t *) base)[3] = (uint8_t) ((entry >> 24) & 0xFF);
    ((uint8_t *) base)[4] = (uint8_t) ((entry >> 32) & 0xFF);
    ((uint8_t *) base)[5] = (uint8_t) ((entry >> 40) & 0xFF);
    ((uint8_t *) base)[6] = (uint8_t) ((entry >> 48) & 0xFF);
    ((uint8_t *) base)[7] = (uint8_t) ((entry >> 56) & 0xFF);
    return;
}

void KAPI hal_write_pdpt_entry(void *const base, uint64_t const pd_addr, uint64_t const attr)
{
    if (base == NULL)
        return;
    uint64_t entry = (pd_addr & 0xFFFFFFFFFF000) | attr;
    ((uint8_t *) base)[0] = (uint8_t) (entry & 0xFF);
    ((uint8_t *) base)[1] = (uint8_t) ((entry >> 8) & 0xFF);
    ((uint8_t *) base)[2] = (uint8_t) ((entry >> 16) & 0xFF);
    ((uint8_t *) base)[3] = (uint8_t) ((entry >> 24) & 0xFF);
    ((uint8_t *) base)[4] = (uint8_t) ((entry >> 32) & 0xFF);
    ((uint8_t *) base)[5] = (uint8_t) ((entry >> 40) & 0xFF);
    ((uint8_t *) base)[6] = (uint8_t) ((entry >> 48) & 0xFF);
    ((uint8_t *) base)[7] = (uint8_t) ((entry >> 56) & 0xFF);
    return;
}

void KAPI hal_write_pml4_entry(void *const base, uint64_t const pdpt_addr, uint64_t const attr)
{
    if (base == NULL)
        return;
    uint64_t const entry = (pdpt_addr & 0xFFFFFFFFFF000) | attr;
    ((uint8_t *) base)[0] = (uint8_t) (entry & 0xFF);
    ((uint8_t *) base)[1] = (uint8_t) ((entry >> 8) & 0xFF);
    ((uint8_t *) base)[2] = (uint8_t) ((entry >> 16) & 0xFF);
    ((uint8_t *) base)[3] = (uint8_t) ((entry >> 24) & 0xFF);
    ((uint8_t *) base)[4] = (uint8_t) ((entry >> 32) & 0xFF);
    ((uint8_t *) base)[5] = (uint8_t) ((entry >> 40) & 0xFF);
    ((uint8_t *) base)[6] = (uint8_t) ((entry >> 48) & 0xFF);
    ((uint8_t *) base)[7] = (uint8_t) ((entry >> 56) & 0xFF);
    return;
}

void KAPI hal_write_segment_descriptor(void *const gdt, uint32_t const base, uint32_t const limit,
                                       uint64_t const attr)
{
    if (gdt == NULL)
        return;
    uint64_t const seg_desc = (((uint64_t) base & 0xFFFF) << 16) | ((((uint64_t) base >> 16) & 0xFF) << 32) |
                              ((((uint64_t) base >> 24) & 0xFF) << 56) | ((uint64_t) limit & 0xFFFF) |
                              ((((uint64_t) limit >> 16) & 0xF) << 48) | attr;
    ((uint8_t *) gdt)[0] = (uint8_t) (seg_desc & 0xFF);
    ((uint8_t *) gdt)[1] = (uint8_t) ((seg_desc >> 8) & 0xFF);
    ((uint8_t *) gdt)[2] = (uint8_t) ((seg_desc >> 16) & 0xFF);
    ((uint8_t *) gdt)[3] = (uint8_t) ((seg_desc >> 24) & 0xFF);
    ((uint8_t *) gdt)[4] = (uint8_t) ((seg_desc >> 32) & 0xFF);
    ((uint8_t *) gdt)[5] = (uint8_t) ((seg_desc >> 40) & 0xFF);
    ((uint8_t *) gdt)[6] = (uint8_t) ((seg_desc >> 48) & 0xFF);
    ((uint8_t *) gdt)[7] = (uint8_t) ((seg_desc >> 56) & 0xFF);
    return;
}

void *KAPI halloc(uint32_t size)
{
    return ke_salloc(kernel_heap, size);
}

void KAPI hfree(void *ptr)
{
    ke_sfree(kernel_heap, ptr);
    return;
}

static void KAPI _hal_init_gdt()
{
    uint32_t coreid = hal_get_core_id();
    // get gdt ready
    hal_write_segment_descriptor((void *) &_gdts[coreid][0], 0, 0, 0);
    hal_write_segment_descriptor((void *) &_gdts[coreid][8], 0, 0,
                                 SEG_DPL_0 | SEG_CODE_DATA | SEG_PRESENT | SEG_LONG | SEG_TYPE_CODE_X);
    hal_write_segment_descriptor((void *) &_gdts[coreid][16], 0, 0,
                                 SEG_DPL_0 | SEG_CODE_DATA | SEG_PRESENT | SEG_LONG | SEG_TYPE_DATA_RW);
    hal_write_segment_descriptor((void *) &_gdts[coreid][24], 0, 0,
                                 SEG_DPL_3 | SEG_CODE_DATA | SEG_PRESENT | SEG_LONG | SEG_TYPE_CODE_X);
    hal_write_segment_descriptor((void *) &_gdts[coreid][32], 0, 0,
                                 SEG_DPL_3 | SEG_CODE_DATA | SEG_PRESENT | SEG_LONG | SEG_TYPE_DATA_RW);

    hal_write_segment_descriptor((void *) &_gdts[coreid][40], 0, 0xFFFFF,
                                 SEG_DPL_0 | SEG_GRANULARITY | SEG_CODE_DATA | SEG_PRESENT | SEG_32_BITS |
                                 SEG_TYPE_CODE_X);
    hal_write_segment_descriptor((void *) &_gdts[coreid][48], 0, 0xFFFFF,
                                 SEG_DPL_0 | SEG_GRANULARITY | SEG_CODE_DATA | SEG_PRESENT | SEG_32_BITS |
                                 SEG_TYPE_DATA_RW);
    hal_write_segment_descriptor((void *) &_gdts[coreid][56], 0, 0xFFFFF,
                                 SEG_DPL_3 | SEG_GRANULARITY | SEG_CODE_DATA | SEG_PRESENT | SEG_32_BITS |
                                 SEG_TYPE_CODE_X);
    hal_write_segment_descriptor((void *) &_gdts[coreid][64], 0, 0xFFFFF,
                                 SEG_DPL_3 | SEG_GRANULARITY | SEG_CODE_DATA | SEG_PRESENT | SEG_32_BITS |
                                 SEG_TYPE_DATA_RW);
    _gdt_ptrs[coreid].base = (uint64_t) &_gdts[coreid];
    _gdt_ptrs[coreid].limit = GDT_ENTRY_NUM * GDT_ENTRY_SIZE - 1;
    hal_flush_gdt(&_gdt_ptrs[coreid], seg_selector(1, 0), seg_selector(2, 0));
};

void KAPI hal_mem_init()
{
    _hal_init_gdt();
    ke_salloc_init(kernel_heap, KERNEL_HEAP_SIZE);
    return;
}
