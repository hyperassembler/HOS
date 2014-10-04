#include "mem32.h"
#include "type32.h"

int32 hk_set_gdt_descriptor(gdt_descriptor* pgdt_descriptor, uint32 base, uint32 limit, uint8 access, uint8 gran)
{
	pgdt_descriptor->base_low = (base & 0xFFFF);
	pgdt_descriptor->base_middle = (base >> 16) & 0xFF;
	pgdt_descriptor->base_high = (base >> 24) & 0xFF;

	pgdt_descriptor->limit_low = (limit & 0xFFFF);
	pgdt_descriptor->access = ((limit >> 16) & 0x0F);

	pgdt_descriptor->granularity |= (gran & 0xF0);
	pgdt_descriptor->access = access;
}