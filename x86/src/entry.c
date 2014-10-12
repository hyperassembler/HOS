#include "type32.h"
#include "grub.h"
#include "mem32.h"
#include "kdef32.h"

uint64 g_gdt[32];
gdt_ptr g_gdt_ptr;

uint32 HKA32 hk_main(multiboot_info_t* multibootInfo)
{
	int32 gdtIdx = 0;
	//empty segment
	hk_set_segment_descriptor(&g_gdt[gdtIdx++], 0x0, 0x0, 0x0, 0x0);
	//ring 0  code segment
	hk_set_segment_descriptor(&g_gdt[gdtIdx++], 0x0, 0xFFFFF, 0x9A, 0xC);
	//ring 3  code segment
	hk_set_segment_descriptor(&g_gdt[gdtIdx++], 0x0, 0xFFFFF, 0xFA, 0xC);
	//ring 0 data
	hk_set_segment_descriptor(&g_gdt[gdtIdx++], 0x0, 0xFFFFF, 0x92, 0xC);
	//ring 3 data
	hk_set_segment_descriptor(&g_gdt[gdtIdx++], 0x0, 0xFFFFF, 0xF2, 0xC);
	g_gdt_ptr.limit = 32*8-1;
	g_gdt_ptr.base = g_gdt;


    //setup new gdt
    //setup paging
    //setup interrupt
    //setup process
    return 0;
}