#include "mem32.h"
#include "kdef32.h"

int32 HYPKERNEL32 hk_set_segment_descriptor(segment_descriptor* pseg_descriptor, uint32 base, uint32 limit, uint8 access, uint8 flags)
{
	if (pseg_descriptor == NULL)
		return -1;
	pseg_descriptor->base_low = (uint16)(base & 0xFFFF);
	pseg_descriptor->base_middle = (uint8)((base >> 16) & 0xFF);
	pseg_descriptor->base_high = (uint8)((base >> 24) & 0xFF);
	
	pseg_descriptor->limit_low = (uint16)(limit & 0xFFFF);
	pseg_descriptor->access = (uint8)(access & 0xFF);
	pseg_descriptor->limit_mid_flags = (uint8)(((flags & 0x0F) << 4) + ((limit >> 16) & 0x0F));
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
	uint32 page_table_base = ;
	uint32 pte_idx = ;
}