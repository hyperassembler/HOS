#include "type.h"
#include "kdef.h"
#include "multiboot.h"
#include "mem.h"
#include "print.h"

uint8 g_gdt[8 * 8];
gdt_ptr g_gdt_ptr;
uint8 g_idt[8 * 256];
idt_ptr g_idt_ptr;
extern uint32 text_pos;
extern int32* kernel_stack;

void HYPKERNEL32 hk_delay(uint32 i)
{
    uint32 j = 100000;
    while(i--)
    {
        while(j > 0)
        {
            j--;
        }
    }
    return;
}

void HYPKERNEL32 hk_main(multiboot_info_t const * const multiboot_info)
{
    //initialize text position
    uint32 i = 0;
    text_pos = 0;
    hk_print_str("Welcome to HYP OS! Please wait while we are gathering information...\n\n");
    hk_print_str("Kernel loaded to address ");
//    hk_print_hex(*kernel_addr);
    hk_print_str(".\n\n");

    segment_descriptor desc_dummy = {.DPL = 0, .Pr = 0, .x64 = 0, .Sys = 0, .type = 0, .Sz = 0, .limit = 0, .Gr = 0, .base = 0, .Acc = 0};
    segment_descriptor desc = {.Gr = 1, .Pr = 1, .Sz = 1, .Acc = 0, .Sys = 1, .x64 = 0, .base = 0, .limit = 0xFFFFF};

    hk_print_str("*Setting up GDT...\n\n");
    //dummy descriptor
    hk_write_segment_descriptor(&g_gdt[0], &desc_dummy);
    //ring 0 code seg, non-conforming
    desc.type = 10;
    desc.DPL = 0;
    hk_write_segment_descriptor(&g_gdt[8], &desc);
    //ring 3 code seg
    desc.DPL = 3;
    hk_write_segment_descriptor(&g_gdt[16], &desc);
    //ring 0 data RW
    desc.DPL = 0;
    desc.type = 2;
    hk_write_segment_descriptor(&g_gdt[24], &desc);
    //ring 3 data
    desc.DPL = 3;
    hk_write_segment_descriptor(&g_gdt[32], &desc);
    g_gdt_ptr.limit = 8 * 8 - 1;
    g_gdt_ptr.base = (uint32) g_gdt;
    hk_load_gdt(&g_gdt_ptr, SEGMENT_SELECTOR(1, 0), SEGMENT_SELECTOR(3, 0));

    //check memory, definitely < 32 so we assume that
    //TODO: Add check for flags and update print functions to handle 64bit int
    hk_print_str("*Checking memory information...\n");
    BOCHS_MAGIC_BREAKPOINT;
    if(multiboot_info->flags & (1 << 6))
    {
        multiboot_memory_map_t const *mem_map = (multiboot_memory_map_t *) multiboot_info->mmap_addr;
        uint32 const mem_map_size = multiboot_info->mmap_length / sizeof(multiboot_memory_map_t);
        hk_print_str("BaseAddr  -  Length  -  Type\n");
        uint32 total_available_mem = 0;
        uint32 total_reserved_mem = 0;
        for (i = 0; i < mem_map_size; i++)
        {
            hk_print_hex((uint32)((mem_map + i)->addr));
            hk_print_str("  -  ");
            hk_print_hex((uint32)((mem_map + i)->len));
            hk_print_str("  -  ");
            hk_print_hex((mem_map + i)->type);
            hk_print_str("\n");
            if((mem_map + i)->type == MULTIBOOT_MEMORY_RESERVED)
            {
                total_reserved_mem += (uint32) ((mem_map + i)->len);
            }
            else if ((mem_map + i)->type == MULTIBOOT_MEMORY_AVAILABLE)
            {
                total_available_mem += (uint32) ((mem_map + i)->len);
            }
        }
        hk_print_str("Total available memory: ");
        hk_print_int(total_available_mem);
        hk_print_str("B = ");
        hk_print_int(total_available_mem/1024);
        hk_print_str("KB = ");
        hk_print_int(total_available_mem/1024/1024);
        hk_print_str("MB\n");
        hk_print_str("Total reserved memory: ");
        hk_print_int(total_reserved_mem);
        hk_print_str("B = ");
        hk_print_int(total_reserved_mem/1024);
        hk_print_str("KB = ");
        hk_print_int(total_reserved_mem/1024/1024);
        hk_print_str("MB\n\n");
    }
    else
    {
        hk_print_str("Memory information is currently unavailable.\n\n");
    }

    //check modules
    hk_print_str("*Checking loaded kernel modules...\n");
    if(multiboot_info->flags & (1 << 3))
    {
        multiboot_module_t const * mods_list = (multiboot_module_t *)multiboot_info->mods_addr;
        uint32 const mods_count = multiboot_info->mods_count;
        hk_print_int(mods_count);
        hk_print_str(" module(s) loaded:\n");
        hk_print_str("Name  -  StartAddr  -  EndAddr\n");
        for (i = 0; i < mods_count; i++)
        {
            hk_print_str((char *) (mods_list + i)->cmdline);
            hk_print_str("  -  ");
            hk_print_hex((mods_list + i)->mod_start);
            hk_print_str("  -  ");
            hk_print_hex((mods_list + i)->mod_end);
            hk_print_str("\n");
        }
        hk_print_str("\n");
    }
    else
    {
        hk_print_str("Module information is currently unavailable.\n\n");
    }


    //detect architecture
    hk_print_str("*Checking architecture...\n");
    if (hk_support_x64() == 0)
    {
        hk_print_str("Arch: x86.");
        x86:
        goto x86;
    }
    else
    {
        hk_print_str("Arch: x86_64.");
    }

    //Setup x64
    x64:
    goto x64;
}


