
#include "cdef.h"
#include "cpu.h"
#include "mem.h"
#include "intr.h"
#include "hal.h"

static uint8 _gdts[HAL_CORE_COUNT][GDT_ENTRY_NUM * GDT_ENTRY_SIZE];
static struct hal_gdt_ptr _gdt_ptrs[HAL_CORE_COUNT];

#define HAL_HEAP_SIZE 8192
static uint32 hal_heap_used;
static char hal_heap[HAL_HEAP_SIZE];

uint32
hal_write_initial_page_table(void *multiboot_info)
{
    UNREFERENCED(multiboot_info);

    /*
    // still identity mapping
    uint32 pt_num = 0;
    uint32 pd_num = 0;
    uint32 pdpt_num = 0;
    uint32 pml4_num = 0;

    // calculate the number of page tables required:
    u64 k_size = (uintptr)KERNEL_IMAGE_END_VADDR - (uintptr)KERNEL_IMAGE_VADDR;
    // see multiboot boot info header
    uint32 m_size = *(uint32 *)multiboot_info;

    // how many pages do we need to hold the entries
    // 512 page table entries per 4k page
    pt_num = (1 + (uint32)((k_size + m_size - 1) / KERNEL_PAGE_SIZE)) / 512;
    pd_num = 1 + (pt_num - 1) / 512;
    pdpt_num = 1 + (pd_num - 1) / 512;
    pml4_num = 1 + (pdpt_num - 1) / 512;

    // calculate the # of page tables
    if ((((uintptr)(pt_end) - (uintptr)(pt_base)) / KERNEL_PAGE_SIZE) < (pt_num + pd_num + pdpt_num + pml4_num))
    {
        return STATUS_FAIL;
    }

    // map kernel first
    KERNEL_IMAGE_VADDR = ;

    // map kernel dynamic
    KERNEL_DYNAMIC_SIZE = ;

    // map recursive page tables
    hal_write_pml4(pt_base, (uintptr)pt_base, PML4_PRESENT | PML4_WRITE);
     */

    return 0;
}


void
hal_write_pt(void *const base, uintptr const p_addr, uint64 const attr)
{
    if (base == NULL)
    {
        return;
    }
    uint64 entry = (p_addr & 0xFFFFFFFFFF000) | attr;
    ((uint8 *) base)[0] = (uint8) (entry & 0xFF);
    ((uint8 *) base)[1] = (uint8) ((entry >> 8) & 0xFF);
    ((uint8 *) base)[2] = (uint8) ((entry >> 16) & 0xFF);
    ((uint8 *) base)[3] = (uint8) ((entry >> 24) & 0xFF);
    ((uint8 *) base)[4] = (uint8) ((entry >> 32) & 0xFF);
    ((uint8 *) base)[5] = (uint8) ((entry >> 40) & 0xFF);
    ((uint8 *) base)[6] = (uint8) ((entry >> 48) & 0xFF);
    ((uint8 *) base)[7] = (uint8) ((entry >> 56) & 0xFF);
}

void
hal_write_pd(void *const base, uintptr const pt_addr, uint64 const attr)
{
    if (base == NULL)
    {
        return;
    }
    uint64 entry = (pt_addr & 0xFFFFFFFFFF000) | attr;
    ((uint8 *) base)[0] = (uint8) (entry & 0xFF);
    ((uint8 *) base)[1] = (uint8) ((entry >> 8) & 0xFF);
    ((uint8 *) base)[2] = (uint8) ((entry >> 16) & 0xFF);
    ((uint8 *) base)[3] = (uint8) ((entry >> 24) & 0xFF);
    ((uint8 *) base)[4] = (uint8) ((entry >> 32) & 0xFF);
    ((uint8 *) base)[5] = (uint8) ((entry >> 40) & 0xFF);
    ((uint8 *) base)[6] = (uint8) ((entry >> 48) & 0xFF);
    ((uint8 *) base)[7] = (uint8) ((entry >> 56) & 0xFF);
}

void
hal_write_pdpt(void *const base, uintptr const pd_addr, uint64 const attr)
{
    if (base == NULL)
    {
        return;
    }
    uint64 entry = (pd_addr & 0xFFFFFFFFFF000) | attr;
    ((uint8 *) base)[0] = (uint8) (entry & 0xFF);
    ((uint8 *) base)[1] = (uint8) ((entry >> 8) & 0xFF);
    ((uint8 *) base)[2] = (uint8) ((entry >> 16) & 0xFF);
    ((uint8 *) base)[3] = (uint8) ((entry >> 24) & 0xFF);
    ((uint8 *) base)[4] = (uint8) ((entry >> 32) & 0xFF);
    ((uint8 *) base)[5] = (uint8) ((entry >> 40) & 0xFF);
    ((uint8 *) base)[6] = (uint8) ((entry >> 48) & 0xFF);
    ((uint8 *) base)[7] = (uint8) ((entry >> 56) & 0xFF);
}

void
hal_write_pml4(void *const base, uintptr const pdpt_addr, uint64 const attr)
{
    if (base == NULL)
    {
        return;
    }
    uint64 const entry = (pdpt_addr & 0xFFFFFFFFFF000) | attr;
    ((uint8 *) base)[0] = (uint8) (entry & 0xFF);
    ((uint8 *) base)[1] = (uint8) ((entry >> 8) & 0xFF);
    ((uint8 *) base)[2] = (uint8) ((entry >> 16) & 0xFF);
    ((uint8 *) base)[3] = (uint8) ((entry >> 24) & 0xFF);
    ((uint8 *) base)[4] = (uint8) ((entry >> 32) & 0xFF);
    ((uint8 *) base)[5] = (uint8) ((entry >> 40) & 0xFF);
    ((uint8 *) base)[6] = (uint8) ((entry >> 48) & 0xFF);
    ((uint8 *) base)[7] = (uint8) ((entry >> 56) & 0xFF);
}

void
hal_write_segment_descriptor(void *const gdt, uint32 const base, uint32 const limit, uint64 const attr)
{
    if (gdt == NULL)
    {
        return;
    }
    uint64 const seg_desc = (((uint64) base & 0xFFFF) << 16) | ((((uint64) base >> 16) & 0xFF) << 32) |
                            ((((uint64) base >> 24) & 0xFF) << 56) | ((uint64) limit & 0xFFFF) |
                            ((((uint64) limit >> 16) & 0xF) << 48) | attr;
    ((uint8 *) gdt)[0] = (uint8) (seg_desc & 0xFF);
    ((uint8 *) gdt)[1] = (uint8) ((seg_desc >> 8) & 0xFF);
    ((uint8 *) gdt)[2] = (uint8) ((seg_desc >> 16) & 0xFF);
    ((uint8 *) gdt)[3] = (uint8) ((seg_desc >> 24) & 0xFF);
    ((uint8 *) gdt)[4] = (uint8) ((seg_desc >> 32) & 0xFF);
    ((uint8 *) gdt)[5] = (uint8) ((seg_desc >> 40) & 0xFF);
    ((uint8 *) gdt)[6] = (uint8) ((seg_desc >> 48) & 0xFF);
    ((uint8 *) gdt)[7] = (uint8) ((seg_desc >> 56) & 0xFF);
}

void hal_init_gdt(void)
{
    uint32 coreid = hal_get_core_id();
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
    _gdt_ptrs[coreid].base = (uint64) &_gdts[coreid];
    _gdt_ptrs[coreid].limit = GDT_ENTRY_NUM * GDT_ENTRY_SIZE - 1;
    hal_flush_gdt(&_gdt_ptrs[coreid], seg_selector(1, 0), seg_selector(2, 0));
}

void
hal_mem_init(struct multiboot_tag_mmap *info)
{
    UNREFERENCED(info);
}

void *
halloc(uint32 size)
{
    void *ret;
    ret = NULL;
    if (hal_heap_used + size < HAL_HEAP_SIZE)
    {
        ret = (void *) ((uintptr) hal_heap + hal_heap_used);
        hal_heap_used += size;
    }
    return ret;
}

void
hfree(void *ptr)
{
    /**
     * Do nothing for now since salloc not available in HAL
     */
    UNREFERENCED(ptr);
}
