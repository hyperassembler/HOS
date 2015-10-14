#include "../common/sys/kdef.h"
#include "../common/sys/type.h"
#include "mem.h"

#define kernel_heap_size 4096
char* _cur_heap = NULL;
extern char kernel_heap[kernel_heap_size];

void _KERNEL_ABI hal_write_pt_entry(void *const base, uint64_t const p_addr, uint64_t const attr)
{
    if(base == NULL)
        return;
    uint64_t entry = (p_addr & 0xFFFFFFFFFF000) | attr;
    ((uint8_t*)base)[0] = (uint8_t)(entry & 0xFF);
    ((uint8_t*)base)[1] = (uint8_t)((entry >> 8) & 0xFF);
    ((uint8_t*)base)[2] = (uint8_t)((entry >> 16) & 0xFF);
    ((uint8_t*)base)[3] = (uint8_t)((entry >> 24) & 0xFF);
    ((uint8_t*)base)[4] = (uint8_t)((entry >> 32) & 0xFF);
    ((uint8_t*)base)[5] = (uint8_t)((entry >> 40) & 0xFF);
    ((uint8_t*)base)[6] = (uint8_t)((entry >> 48) & 0xFF);
    ((uint8_t*)base)[7] = (uint8_t)((entry >> 56) & 0xFF);
    return;
}

void _KERNEL_ABI hal_write_pd_entry(void *const base, uint64_t const pt_addr, uint64_t const attr)
{
    if(base == NULL)
        return;
    uint64_t entry = (pt_addr & 0xFFFFFFFFFF000) | attr;
    ((uint8_t*)base)[0] = (uint8_t)(entry & 0xFF);
    ((uint8_t*)base)[1] = (uint8_t)((entry >> 8) & 0xFF);
    ((uint8_t*)base)[2] = (uint8_t)((entry >> 16) & 0xFF);
    ((uint8_t*)base)[3] = (uint8_t)((entry >> 24) & 0xFF);
    ((uint8_t*)base)[4] = (uint8_t)((entry >> 32) & 0xFF);
    ((uint8_t*)base)[5] = (uint8_t)((entry >> 40) & 0xFF);
    ((uint8_t*)base)[6] = (uint8_t)((entry >> 48) & 0xFF);
    ((uint8_t*)base)[7] = (uint8_t)((entry >> 56) & 0xFF);
    return;
}

void _KERNEL_ABI hal_write_pdpt_entry(void *const base, uint64_t const pd_addr, uint64_t const attr)
{
    if(base == NULL)
        return;
    uint64_t entry = (pd_addr & 0xFFFFFFFFFF000) | attr;
    ((uint8_t*)base)[0] = (uint8_t)(entry & 0xFF);
    ((uint8_t*)base)[1] = (uint8_t)((entry >> 8) & 0xFF);
    ((uint8_t*)base)[2] = (uint8_t)((entry >> 16) & 0xFF);
    ((uint8_t*)base)[3] = (uint8_t)((entry >> 24) & 0xFF);
    ((uint8_t*)base)[4] = (uint8_t)((entry >> 32) & 0xFF);
    ((uint8_t*)base)[5] = (uint8_t)((entry >> 40) & 0xFF);
    ((uint8_t*)base)[6] = (uint8_t)((entry >> 48) & 0xFF);
    ((uint8_t*)base)[7] = (uint8_t)((entry >> 56) & 0xFF);
    return;
}

void _KERNEL_ABI hal_write_pml4_entry(void *const base, uint64_t const pdpt_addr, uint64_t const attr)
{
    if(base == NULL)
        return;
    uint64_t const entry = (pdpt_addr & 0xFFFFFFFFFF000) | attr;
    ((uint8_t*)base)[0] = (uint8_t)(entry & 0xFF);
    ((uint8_t*)base)[1] = (uint8_t)((entry >> 8) & 0xFF);
    ((uint8_t*)base)[2] = (uint8_t)((entry >> 16) & 0xFF);
    ((uint8_t*)base)[3] = (uint8_t)((entry >> 24) & 0xFF);
    ((uint8_t*)base)[4] = (uint8_t)((entry >> 32) & 0xFF);
    ((uint8_t*)base)[5] = (uint8_t)((entry >> 40) & 0xFF);
    ((uint8_t*)base)[6] = (uint8_t)((entry >> 48) & 0xFF);
    ((uint8_t*)base)[7] = (uint8_t)((entry >> 56) & 0xFF);
    return;
}

void _KERNEL_ABI hal_write_segment_descriptor(void *const gdt, uint32_t const base, uint32_t const limit, uint64_t const attr)
{
    if (gdt == NULL)
        return;
    uint64_t const seg_desc = (((uint64_t)base & 0xFFFF) << 16) | ((((uint64_t)base >> 16) & 0xFF) << 32) | ((((uint64_t)base >> 24) & 0xFF) << 56) | ((uint64_t)limit & 0xFFFF) | ((((uint64_t)limit >> 16) & 0xF) << 48) | attr;
    ((uint8_t*)gdt)[0] = (uint8_t)(seg_desc & 0xFF);
    ((uint8_t*)gdt)[1] = (uint8_t)((seg_desc >> 8) & 0xFF);
    ((uint8_t*)gdt)[2] = (uint8_t)((seg_desc >> 16) & 0xFF);
    ((uint8_t*)gdt)[3] = (uint8_t)((seg_desc >> 24) & 0xFF);
    ((uint8_t*)gdt)[4] = (uint8_t)((seg_desc >> 32) & 0xFF);
    ((uint8_t*)gdt)[5] = (uint8_t)((seg_desc >> 40) & 0xFF);
    ((uint8_t*)gdt)[6] = (uint8_t)((seg_desc >> 48) & 0xFF);
    ((uint8_t*)gdt)[7] = (uint8_t)((seg_desc >> 56) & 0xFF);
    return;
}

void _KERNEL_ABI hal_create_initial_page_table(void* const base, uint64_t size)
{

};


//uint64_t _KERNEL_ABI hal_map_page(void* const base, uint64_t const p_addr, uint64_t const v_addr, uint64_t const flags)
//{
//    // assume the initial page table has already been allocated
//
//    // check p_addr and v_addr 4k-aligned
//    if(base == NULL || p_addr << 52 || v_addr << 52)
//        return 1;
//
//    uint64_t const pml4_index = (v_addr >> 39) & 0x1FF;
//    uint64_t const pdpt_index = (v_addr >> 30) & 0x1FF;
//    uint64_t const pd_index = (v_addr >> 21) & 0x1FF;
//    uint64_t const pt_index = (v_addr >> 12) & 0x1FF;
//
//    void * const pml4_entry_addr = (void*)((uint64_t*) base + pml4_index);
//    if(!(*(uint64_t*)pml4_entry_addr & PML4_PRESENT))
//    {
//        //PML4 does not exist
//        return 1;
//    }
//    uint64_t const pml4_entry = *(uint64_t*)pml4_entry_addr;
//
//    void * const pdpt_entry_addr = (void*)((uint64_t*) PAGE_ENTRY_BASE(pml4_entry) + pdpt_index);
//    if(!(*(uint64_t*) pdpt_entry_addr & PDPT_PRESENT))
//    {
//        //PDPT does not exist
//        return 1;
//    }
//
//    uint64_t const pdpt_entry = *(uint64_t*)pdpt_entry_addr;
//
//    void * const pd_entry_addr = (void*)((uint64_t*) PAGE_ENTRY_BASE(pdpt_entry) + pd_index);
//    if(!(*(uint64_t*) pd_entry_addr & PD_PRESENT))
//    {
//        write_pd_entry(pd_entry_addr, (uint64_t)((uint64_t*)pt_base + pml4_index * 512 * 512 * 512 + pdpt_index * 512 * 512 + pd_index*512), PD_PRESENT | PD_WRITE);
//    }
//    uint64_t const pd_entry = *(uint64_t*)pd_entry_addr;
//
//    void * const pt_entry_addr = (void*)((uint64_t*) PAGE_ENTRY_BASE(pd_entry) + pt_index);
//    hal_write_pt_entry(pt_entry_addr, p_addr, flags);
//    return 0;
//}

void*_KERNEL_ABI hal_halloc(size_t const size)
{
    if(_cur_heap == NULL)
        _cur_heap = kernel_heap;
    if(_cur_heap + size < kernel_heap + kernel_heap_size)
    {
        void * temp_heap = (void*)_cur_heap;
        _cur_heap = _cur_heap + size;
        return temp_heap;
    }
    return NULL;
}

void _KERNEL_ABI hal_hfree(void *ptr)
{
    return;
}