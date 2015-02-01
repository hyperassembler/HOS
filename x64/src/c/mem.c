#include "kdef.h"
#include "mem.h"

void HYPKERNEL64 hk_write_pml4_entry(uint8_t * const base, pml4_entry_t const * const p_entry)
{
    if(base == NULL || p_entry == NULL)
        return;
    base[0] = (uint8_t)((p_entry->Pr & 0x1) + ((p_entry->RW & 0x1) << 1)  + ((p_entry->USU & 0x1) << 2)  + ((p_entry->PWT & 0x1) << 3) + ((p_entry->PCD & 0x1) << 4) + ((p_entry->Acc & 0x1) << 5) + ((p_entry->Sz & 0x1) << 7));
    base[1] = (uint8_t)(((p_entry->base >> 12) & 0xF) << 4); // 4 bits
    base[2] = (uint8_t)((p_entry->base >> 16) & 0xFF); // 8 bits
    base[3] = (uint8_t)((p_entry->base >> 24) & 0xFF); // 8 bits
    base[4] = (uint8_t)((p_entry->base >> 32) & 0xFF); // 8 bits
    base[5] = (uint8_t)((p_entry->base >> 40) & 0xFF); // 8 bits
    base[6] = (uint8_t)((p_entry->base >> 48) & 0xF); // 4 bits
    base[7] = (uint8_t)((p_entry->XD & 0x1) << 7);
    return;
}

void HYPKERNEL64 hk_write_pdpt_entry(uint8_t * const base, pdpt_entry_t const * const p_entry)
{
    if(base == NULL || p_entry == NULL)
        return;
    base[0] = (uint8_t)((p_entry->Pr & 0x1) + ((p_entry->RW & 0x1) << 1)  + ((p_entry->USU & 0x1) << 2)  + ((p_entry->PWT & 0x1) << 3) + ((p_entry->PCD & 0x1) << 4) + ((p_entry->Acc & 0x1) << 5) + ((p_entry->Sz & 0x1) << 7));
    base[1] = (uint8_t)(((p_entry->base >> 12) & 0xF) << 4); // 4 bits
    base[2] = (uint8_t)((p_entry->base >> 16) & 0xFF); // 8 bits
    base[3] = (uint8_t)((p_entry->base >> 24) & 0xFF); // 8 bits
    base[4] = (uint8_t)((p_entry->base >> 32) & 0xFF); // 8 bits
    base[5] = (uint8_t)((p_entry->base >> 40) & 0xFF); // 8 bits
    base[6] = (uint8_t)((p_entry->base >> 48) & 0xF); // 4 bits
    base[7] = (uint8_t)((p_entry->XD & 0x1) << 7);
    return;
}

void HYPKERNEL64 hk_write_pd_entry(uint8_t * const base, pd_entry_t const * const p_entry)
{
    if(base == NULL || p_entry == NULL)
        return;
    base[0] = (uint8_t)((p_entry->Pr & 0x1) + ((p_entry->RW & 0x1) << 1)  + ((p_entry->USU & 0x1) << 2)  + ((p_entry->PWT & 0x1) << 3) + ((p_entry->PCD & 0x1) << 4) + ((p_entry->Acc & 0x1) << 5) + ((p_entry->Sz & 0x1) << 7));
    base[1] = (uint8_t)(((p_entry->base >> 12) & 0xF) << 4); // 4 bits
    base[2] = (uint8_t)((p_entry->base >> 16) & 0xFF); // 8 bits
    base[3] = (uint8_t)((p_entry->base >> 24) & 0xFF); // 8 bits
    base[4] = (uint8_t)((p_entry->base >> 32) & 0xFF); // 8 bits
    base[5] = (uint8_t)((p_entry->base >> 40) & 0xFF); // 8 bits
    base[6] = (uint8_t)((p_entry->base >> 48) & 0xF); // 4 bits
    base[7] = (uint8_t)((p_entry->XD & 0x1) << 7);
    return;
}


void HYPKERNEL64 hk_write_pt_entry(uint8_t * const base, pt_entry_t const * const p_entry)
{
    if(base == NULL || p_entry == NULL)
        return;
    base[0] = (uint8_t)((p_entry->Pr & 0x1) + ((p_entry->RW & 0x1) << 1)  + ((p_entry->USU & 0x1) << 2)  + ((p_entry->PWT & 0x1) << 3) + ((p_entry->PCD & 0x1) << 4) + ((p_entry->Acc & 0x1) << 5) + ((p_entry->dirty & 0x1) << 6) + ((p_entry->PAT & 0x1) << 7));
    base[1] = (uint8_t)((((p_entry->base >> 12) & 0xF) << 4) + (p_entry->Gl & 0x1)); // 4 bits
    base[2] = (uint8_t)((p_entry->base >> 16) & 0xFF); // 8 bits
    base[3] = (uint8_t)((p_entry->base >> 24) & 0xFF); // 8 bits
    base[4] = (uint8_t)((p_entry->base >> 32) & 0xFF); // 8 bits
    base[5] = (uint8_t)((p_entry->base >> 40) & 0xFF); // 8 bits
    base[6] = (uint8_t)((p_entry->base >> 48) & 0xF); // 4 bits
    base[7] = (uint8_t)((p_entry->XD & 0x1) << 7);
    return;
}

void HYPKERNEL64 hk_write_segment_descriptor(uint8_t *const gdt, segment_descriptor_t const *const seg_desc)
{
	if (gdt == NULL || seg_desc == NULL)
		return;
    gdt[0] = (uint8_t)(seg_desc->limit & 0xFF);
    gdt[1] = (uint8_t)((seg_desc->limit >> 8) & 0xFF);
    gdt[6] =  gdt[6] | (uint8_t)((seg_desc->limit >> 16) & 0xFF);

    gdt[2] = (uint8_t)(seg_desc->base & 0xFF);
    gdt[3] = (uint8_t)((seg_desc->base >> 8) & 0xFF);
    gdt[4] = (uint8_t)((seg_desc->base >> 16) & 0xFF);
    gdt[7] = (uint8_t)((seg_desc->base >> 24) & 0xFF);

    gdt[5] = (uint8_t)((seg_desc->type & 0xF) + ((seg_desc->Sys & 0x1) << 4) + ((seg_desc->DPL & 0x3) << 5) + ((seg_desc->Pr & 0x1 ) << 7));
    gdt[6] = gdt[6] | (uint8_t)(((seg_desc->Acc & 0x1) + ((seg_desc->x64 & 0x1) << 1) + ((seg_desc->Sz & 0x1) << 2) + ((seg_desc->Gr & 0x1) << 3)) << 4);
	return;
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