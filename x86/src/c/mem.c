#include "kdef.h"
#include "mem.h"

void HYPKERNEL32 hk_write_pml4_entry(uint8_t * const base, pml4_entry_t const * const p_entry)
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

void HYPKERNEL32 hk_write_pdpt_entry(uint8_t * const base, pdpt_entry_t const * const p_entry)
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

void HYPKERNEL32 hk_write_pd_entry(uint8_t * const base, pd_entry_t const * const p_entry)
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


void HYPKERNEL32 hk_write_pt_entry(uint8_t * const base, pt_entry_t const * const p_entry)
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

void HYPKERNEL32 hk_write_segment_descriptor(uint8_t *const gdt, segment_descriptor_t const *const seg_desc)
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

void HYPKERNEL32 hk_write_interrupt_gate(uint8_t *const dst, interrupt_gate_t const *int_gate)
{
    if(dst == NULL || int_gate == NULL)
        return;
    dst[0] = (uint8_t)(int_gate->offset & 0xFF);
    dst[1] = (uint8_t)((int_gate->offset >> 8) & 0xFF);
    dst[6] = (uint8_t)((int_gate->offset >> 16) & 0xFF);
    dst[7] = (uint8_t)((int_gate->offset >> 24) & 0xFF);
    dst[2] = (uint8_t)(int_gate->seg_sel & 0xFF);
    dst[3] = (uint8_t)((int_gate->seg_sel >> 8) & 0xFF);;
    dst[4] = 0;
    dst[5] = (uint8_t)(6 + ((int_gate->Sz & 0x1) << 3) + ((int_gate->DPL & 0x3) << 5) + ((int_gate->Pr & 0x1 ) << 7));
    return;
}

void HYPKERNEL32 hk_write_trap_gate(uint8_t *const dst, trap_gate_t const *tr_gate)
{
    if(dst == NULL || tr_gate == NULL)
        return;
    dst[0] = (uint8_t)(tr_gate->offset & 0xFF);
    dst[1] = (uint8_t)((tr_gate->offset >> 8) & 0xFF);
    dst[6] = (uint8_t)((tr_gate->offset >> 16) & 0xFF);
    dst[7] = (uint8_t)((tr_gate->offset >> 24) & 0xFF);
    dst[2] = (uint8_t)(tr_gate->seg_sel & 0xFF);
    dst[3] = (uint8_t)((tr_gate->seg_sel >> 8) & 0xFF);;
    dst[4] = 0;
    dst[5] = (uint8_t)(7 + ((tr_gate->Sz & 0x1) << 3) + ((tr_gate->DPL & 0x3) << 5) + ((tr_gate->Pr & 0x1 ) << 7));
    return;
}

void HYPKERNEL32 hk_write_task_gate(uint8_t *const dst, task_gate_t const *int_gate)
{
    if(dst == NULL || int_gate == NULL)
        return;
    dst[0] = 0;
    dst[1] = 0;
    dst[6] = 0;
    dst[7] = 0;
    dst[2] = (uint8_t)(int_gate->tss_sel & 0xFF);
    dst[3] = (uint8_t)((int_gate->tss_sel >> 8) & 0xFF);
    dst[4] = 0;
    dst[5] = (uint8_t)(5 + ((int_gate->DPL & 0x3) << 5) + ((int_gate->Pr & 0x1 ) << 7));
    return;
}

void HYPKERNEL32 hk_mem_cpy(void* src, void* dst, uint32_t size)
{
    if (src == NULL || dst == NULL)
        return;
    char* cSrc = (char*)src;
    char* cDst = (char*)dst;
    while (size--)
        *(cDst++) = *(cSrc++);
    return;
}

void HYPKERNEL32 hk_mem_set(void* src, int8_t const val,uint32_t size)
{
    if (src == NULL)
        return;
    int8_t * cSrc = (int8_t *)src;
    while (size--)
        *(cSrc++) = val;
    return;
}

void HYPKERNEL32 hk_mem_move(void* src, void* dst, uint32_t size)
{
    if (src == NULL || dst == NULL)
        return;
    char* cSrc = (char*)src;
    char* cDst = (char*)dst;
    if (cSrc >= cDst)
    {
        return hk_mem_cpy(src,dst,size);
    }
    cSrc += size;
    cDst += size;
    while (size--)
        *(--cDst) = *(--cSrc);
    return;
}