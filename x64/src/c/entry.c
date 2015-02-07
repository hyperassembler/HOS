#include "type.h"
#include "kdef.h"
#include "print.h"
#include "mem.h"
#include "multiboot.h"
uint8_t g_gdt[8*9];
gdt_ptr_t g_gdt_ptr;
extern uint64_t text_pos;
extern void HYPKERNEL64 HLT_CPU(void);
extern void HYPKERNEL64 BOCHS_MAGIC_BREAKPOINT();
extern void HYPKERNEL64 hk_flush_gdt(gdt_ptr_t* gdt_ptr, uint64_t code_slct, uint64_t data_slct);
void HYPKERNEL64 hk_main(multiboot_info_t* multiboot_info)
{
    text_pos = 0;
    hk_printf("Kernel is now running in x64 mode. Multiboot info is located at %X.\n\n", (uint64_t)multiboot_info);
    hk_printf("*Setting up GDT...");
    hk_write_segment_descriptor((void*)&g_gdt[0], 0, 0, 0);
    hk_write_segment_descriptor((void*)&g_gdt[8], 0, 0, SEG_DPL_0 | SEG_CODE_DATA | SEG_PRESENT | SEG_LONG | SEG_TYPE_CODE_X);
    hk_write_segment_descriptor((void*)&g_gdt[16], 0, 0, SEG_DPL_0 | SEG_CODE_DATA | SEG_PRESENT | SEG_LONG | SEG_TYPE_DATA_RW);
    hk_write_segment_descriptor((void*)&g_gdt[24], 0, 0, SEG_DPL_3 | SEG_CODE_DATA | SEG_PRESENT | SEG_LONG | SEG_TYPE_CODE_X);
    hk_write_segment_descriptor((void*)&g_gdt[32], 0, 0, SEG_DPL_3 | SEG_CODE_DATA | SEG_PRESENT | SEG_LONG | SEG_TYPE_DATA_RW);

    hk_write_segment_descriptor((void*)&g_gdt[40], 0, 0xFFFFF, SEG_DPL_0 | SEG_GRANULARITY | SEG_CODE_DATA | SEG_PRESENT | SEG_32_BITS | SEG_TYPE_CODE_X);
    hk_write_segment_descriptor((void*)&g_gdt[48], 0, 0xFFFFF, SEG_DPL_0 | SEG_GRANULARITY | SEG_CODE_DATA | SEG_PRESENT | SEG_32_BITS | SEG_TYPE_DATA_RW);
    hk_write_segment_descriptor((void*)&g_gdt[56], 0, 0xFFFFF, SEG_DPL_3 | SEG_GRANULARITY | SEG_CODE_DATA | SEG_PRESENT | SEG_32_BITS | SEG_TYPE_CODE_X);
    hk_write_segment_descriptor((void*)&g_gdt[64], 0, 0xFFFFF, SEG_DPL_3 | SEG_GRANULARITY | SEG_CODE_DATA | SEG_PRESENT | SEG_32_BITS | SEG_TYPE_DATA_RW);

    g_gdt_ptr.base = (uint64_t)g_gdt;
    g_gdt_ptr.limit = 8*9-1;
    hk_flush_gdt(&g_gdt_ptr, SEG_SELECTOR(1, 0), SEG_SELECTOR(2, 0));
    hk_printf("Done.\n\n");
    HLT_CPU();
}
