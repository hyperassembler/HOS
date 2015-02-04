#include "kdef.h"
#include "mem.h"


void HYPKERNEL64 hk_write_pt_entry(void * const base, uint64_t const p_addr, uint64_t const attr)
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

void HYPKERNEL64 hk_write_pd_entry(void * const base, uint64_t const pt_addr, uint64_t const attr)
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

void HYPKERNEL64 hk_write_pdpt_entry(void * const base, uint64_t const pd_addr, uint64_t const attr)
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

void HYPKERNEL64 hk_write_pml4_entry(void * const base, uint64_t const pdpt_addr, uint64_t const attr)
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

void HYPKERNEL64 hk_write_segment_descriptor(void * const gdt, uint32_t const base, uint32_t const limit, uint64_t const attr)
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

void HYPKERNEL64 hk_map_page(void * const pml4_base, uint64_t const p_addr, uint64_t const v_addr, uint64_t const attr)
{
    //wait a sec, we actually need maximum memory information here for effectively map crap
    if(pml4_base == NULL || p_addr << 52 || v_addr << 52)
        return;
    //ASSUME: little endian
    uint64_t const pml4_index = (v_addr >> 39) & 0x1FF;
    void * const pml4_addr = (void*)((uint64_t*)pml4_base + pml4_index);
    uint64_t const pml4_entry = *(uint64_t*)pml4_addr;
    if(!(pml4_entry & PML4_PRESENT))
    {
        hk_write_pml4_entry(pml4_addr, (uint64_t)pml4_base + 512*8, PML4_PRESENT | PML4_WRITE);
    }
}

void HYPKERNEL64 hk_mem_cpy(void* src, void* dst, uint64_t size)
{
    if (src == NULL || dst == NULL)
        return;
    char* cSrc = (char*)src;
    char* cDst = (char*)dst;
    while (size--)
        *(cDst++) = *(cSrc++);
    return;
}

void HYPKERNEL64 hk_mem_set(void* src, int8_t const val,uint64_t size)
{
    if (src == NULL)
        return;
    while (size--)
        *((int8_t*)src++) = val;
    return;
}

void HYPKERNEL64 hk_mem_move(void* src, void* dst, uint64_t size)
{
    if (src == NULL || dst == NULL)
        return;
    if (src >= dst)
    {
        return hk_mem_cpy(src,dst,size);
    }
    src += size;
    dst += size;
    while (size--)
        *((char*)--dst) = *((char*)--src);
    return;
}