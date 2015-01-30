#include "kdef.h"
#include "mem.h"

int32 HYPKERNEL32 hk_set_segment_descriptor(uint8* const gdt, const segment_descriptor* const seg_desc)
{
	if (gdt == NULL || seg_desc == NULL)
		return -1;
    gdt[0] = (uint8)(seg_desc->limit & 0xFF);
    gdt[1] = (uint8)((seg_desc->limit >> 8) & 0xFF);
    gdt[6] =  gdt[6] | (uint8)((seg_desc->limit >> 16) & 0xFF);

    gdt[2] = (uint8)(seg_desc->base & 0xFF);
    gdt[3] = (uint8)((seg_desc->base >> 8) & 0xFF);
    gdt[4] = (uint8)((seg_desc->base >> 16) & 0xFF);
    gdt[7] = (uint8)((seg_desc->base >> 24) & 0xFF);

    gdt[5] = (uint8)((seg_desc->type & 0xF) + ((seg_desc->Sys & 0x1) << 4) + ((seg_desc->DPL & 0x3) << 5) + ((seg_desc->Pr & 0x1 ) << 7));
    gdt[6] = gdt[6] | (uint8)(((seg_desc->Avl & 0x1) + ((seg_desc->x64 & 0x1) << 1) + ((seg_desc->Sz & 0x1) << 2) + ((seg_desc->Gr & 0x1) << 3)) << 4);
	return 0;
}

int32 HYPKERNEL32 hk_set_interrupt_gate(uint8* const dst, const interrupt_gate* int_gate)
{
    if(dst == NULL || int_gate == NULL)
        return -1;
    dst[0] = (uint8)(int_gate->offset & 0xFF);
    dst[1] = (uint8)((int_gate->offset >> 8) & 0xFF);
    dst[6] = (uint8)((int_gate->offset >> 16) & 0xFF);
    dst[7] = (uint8)((int_gate->offset >> 24) & 0xFF);
    dst[2] = (uint8)(int_gate->seg_sel & 0xFF);
    dst[3] = (uint8)((int_gate->seg_sel >> 8) & 0xFF);;
    dst[4] = 0;
    dst[5] = (uint8)(6 + ((int_gate->Sz & 0x1) << 3) + ((int_gate->DPL & 0x3) << 5) + ((int_gate->Pr & 0x1 ) << 7));
    return 0;
}

int32 HYPKERNEL32 hk_set_trap_gate(uint8* const dst, const trap_gate* tr_gate)
{
    if(dst == NULL || tr_gate == NULL)
        return -1;
    dst[0] = (uint8)(tr_gate->offset & 0xFF);
    dst[1] = (uint8)((tr_gate->offset >> 8) & 0xFF);
    dst[6] = (uint8)((tr_gate->offset >> 16) & 0xFF);
    dst[7] = (uint8)((tr_gate->offset >> 24) & 0xFF);
    dst[2] = (uint8)(tr_gate->seg_sel & 0xFF);
    dst[3] = (uint8)((tr_gate->seg_sel >> 8) & 0xFF);;
    dst[4] = 0;
    dst[5] = (uint8)(7 + ((tr_gate->Sz & 0x1) << 3) + ((tr_gate->DPL & 0x3) << 5) + ((tr_gate->Pr & 0x1 ) << 7));
    return 0;
}

int32 HYPKERNEL32 hk_set_task_gate(uint8* const dst, const task_gate* int_gate)
{
    if(dst == NULL || int_gate == NULL)
        return -1;
    dst[0] = 0;
    dst[1] = 0;
    dst[6] = 0;
    dst[7] = 0;
    dst[2] = (uint8)(int_gate->tss_sel & 0xFF);
    dst[3] = (uint8)((int_gate->tss_sel >> 8) & 0xFF);
    dst[4] = 0;
    dst[5] = (uint8)(5 + ((int_gate->DPL & 0x3) << 5) + ((int_gate->Pr & 0x1 ) << 7));
    return 0;
}


int32 HYPKERNEL32 hk_set_page_table_entry_32(uint32* dest,uint32 base,uint32 flags)
{
	if (dest == NULL)
		return -1;
	*dest = (base & 0xFFFFF000) + (flags & 0x0FFF);
	return 0;
}

int32 HYPKERNEL32 hk_set_page_directory_entry_32(uint32* dest, uint32 base, uint32 flags)
{
	if (dest == NULL)
		return -1;
	*dest = (base & 0xFFFFF000) + (flags & 0x0FFF);
	return 0;
}
 
int32 HYPKERNEL32 hk_map_physcial_address_32(uint32* page_directory_base, uint32 physical_addr, uint32 virtual_addr, uint32 flags)
{
	if (page_directory_base == NULL)
		return -1;
	uint32 pde_idx = virtual_addr >> 22;
	uint32 pde = *(page_directory_base + pde_idx * 4);
	//uint32 page_table_base = ;
	//uint32 pte_idx = ;
	return 0;
}
