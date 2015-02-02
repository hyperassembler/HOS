#include "type.h"
#include "kdef.h"
#include "multiboot.h"
#include "mem.h"
#include "print.h"
uint8_t g_gdt[8 * 8];
gdt_ptr_t g_gdt_ptr;
gdt_ptr_64_t g_gdt_ptr_64;

//x86 stuff
uint8_t g_idt[8 * 256];
idt_ptr_t g_idt_ptr;

extern uint32_t text_pos;
extern void hk_entry_comp(void);

void HYPKERNEL32 hk_init_x86(multiboot_info_t const * const multiboot_info)
{
    segment_descriptor_t desc_dummy = {.DPL = 0, .Pr = 0, .x64 = 0, .Sys = 0, .type = 0, .Sz = 0, .limit = 0, .Gr = 0, .base = 0, .Acc = 0};
    segment_descriptor_t desc = {.Gr = 1, .Pr = 1, .Sz = 1, .Acc = 0, .Sys = 1, .x64 = 0, .base = 0, .limit = 0xFFFFF};

    hk_print_str("*Setting up GDT...");
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
    g_gdt_ptr.base = (uint32_t) g_gdt;
    hk_load_gdt(&g_gdt_ptr, SEGMENT_SELECTOR(1, 0), SEGMENT_SELECTOR(3, 0));
    hk_print_str(" - Done.\n\n");

    //check memory, definitely < 32 so we assume that
    hk_print_str("*Checking memory information...\n");
    if(multiboot_info->flags & (1 << 6))
    {
        multiboot_memory_map_t const *mem_map = (multiboot_memory_map_t *) multiboot_info->mmap_addr;
        uint32_t const mem_map_size = multiboot_info->mmap_length / sizeof(multiboot_memory_map_t);
        hk_print_str("BaseAddr  -  Length  -  Type\n");
        uint32_t total_available_mem = 0;
        uint32_t total_reserved_mem = 0;
        uint32_t i = 0;
        for (i = 0; i < mem_map_size; i++)
        {
            hk_print_hex((uint32_t)((mem_map + i)->addr));
            hk_print_str("  -  ");
            hk_print_hex((uint32_t)((mem_map + i)->len));
            hk_print_str("  -  ");
            hk_print_hex((mem_map + i)->type);
            hk_print_str("\n");
            if((mem_map + i)->type == MULTIBOOT_MEMORY_RESERVED)
            {
                total_reserved_mem += (uint32_t) ((mem_map + i)->len);
            }
            else if ((mem_map + i)->type == MULTIBOOT_MEMORY_AVAILABLE)
            {
                total_available_mem += (uint32_t) ((mem_map + i)->len);
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
        uint32_t const mods_count = multiboot_info->mods_count;
        hk_print_int(mods_count);
        hk_print_str(" module(s) loaded:\n");
        hk_print_str("Name  -  StartAddr  -  EndAddr\n");
        uint32_t i = 0;
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
    HLT_CPU
}

void HYPKERNEL32 hk_init_x64(multiboot_info_t const * const multiboot_info)
{
    //Setup x64
    segment_descriptor_t desc_dummy = {.DPL = 0, .Pr = 0, .x64 = 0, .Sys = 0, .type = 0, .Sz = 0, .limit = 0, .Gr = 0, .base = 0, .Acc = 0};
    segment_descriptor_t desc = {.Gr = 1, .Pr = 1, .Sz = 0, .Acc = 0, .Sys = 1, .x64 = 1, .base = 0, .limit = 0};

    hk_print_str("*Setting up GDT ...");
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
    g_gdt_ptr_64.limit = 8 * 8 - 1;
    g_gdt_ptr_64.base = (uint64_t)g_gdt;
    hk_print_str(" - Done.\n\n");

    //check memory, definitely < 32 so we assume that
    hk_print_str("*Checking memory information...\n");
    if(multiboot_info->flags & (1 << 6))
    {
        multiboot_memory_map_t const *mem_map = (multiboot_memory_map_t *) multiboot_info->mmap_addr;
        uint32_t const mem_map_size = multiboot_info->mmap_length / sizeof(multiboot_memory_map_t);
        hk_print_str("BaseAddr  -  Length  -  Type\n");
        uint32_t total_available_mem = 0;
        uint32_t total_reserved_mem = 0;
        uint32_t i = 0;
        for (i = 0; i < mem_map_size; i++)
        {
            hk_print_hex((uint32_t)((mem_map + i)->addr));
            hk_print_str("  -  ");
            hk_print_hex((uint32_t)((mem_map + i)->len));
            hk_print_str("  -  ");
            hk_print_hex((mem_map + i)->type);
            hk_print_str("\n");
            if((mem_map + i)->type == MULTIBOOT_MEMORY_RESERVED)
            {
                total_reserved_mem += (uint32_t) ((mem_map + i)->len);
            }
            else if ((mem_map + i)->type == MULTIBOOT_MEMORY_AVAILABLE)
            {
                total_available_mem += (uint32_t) ((mem_map + i)->len);
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
        uint32_t const mods_count = multiboot_info->mods_count;
        hk_print_int(mods_count);
        hk_print_str(" module(s) loaded:\n");
        hk_print_str("Name  -  StartAddr  -  EndAddr\n");
        uint32_t i = 0;
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


    //Setup identity x64 PAE paging
    hk_print_str("*Setting up paging for x64...");
    //Setup
    //TODO: check for available memory and only allocate stuff needed(should not be really hard)
    uint32_t addr = 0x200000;
    pml4_entry_t pml4_entry = {.base = 0x201000,.Acc = 0, .PWT = 1, .PCD = 0,.Pr = 1,.USU = 0, .XD = 0, .RW = 1, .Sz = 0};
    while(addr < 0x201000)
    {
        hk_write_pml4_entry((uint8_t *) addr,&pml4_entry);
        //only map first 512 gigs for obvious reasons.
        pml4_entry.base = 0;
        pml4_entry.Pr = 0;
        addr += 8;
    }
    addr = 0x201000;
    pdpt_entry_t pdpt_entry = {.base = 0x202000,.Acc = 0, .PWT = 1, .PCD = 0,.Pr = 1,.USU = 0, .XD = 0, .RW = 1, .Sz = 0};
    while(addr < 0x202000)
    {
        hk_write_pdpt_entry((uint8_t*) addr, &pdpt_entry);
        //only map first 1 gig for obvious reasons.
        pdpt_entry.Pr = 0;
        pdpt_entry.base = 0;
        addr += 8;
    }
    addr = 0x202000;
    pd_entry_t pd_entry = {.base = 0x203000,.Acc = 0, .PWT = 1, .PCD = 0,.Pr = 1,.USU = 0, .XD = 0, .RW = 1, .Sz = 0};
    while(addr < 0x203000)
    {
        hk_write_pd_entry((uint8_t *) addr, &pd_entry);
        //ah. no more laziness. At least we need to map first gig.
        pd_entry.base += 0x1000; // increment for every 512 entries * 8 bytes each
        addr += 8;
    }
    addr = 0x203000;
    pt_entry_t pt_entry = {.base = 0, .Acc = 0, .Pr = 1, .RW = 1, .USU = 0, .PWT = 1,.PCD = 0, .dirty = 0,.Gl = 0, .PAT = 0,.XD = 0};
    while(addr < (0x203000 + 512 * 0x1000))
    {
        hk_write_pt_entry((uint8_t *) addr, &pt_entry);
        pt_entry.base += 0x1000;
        addr += 8;
    }
    hk_print_str(" - Done.\n\n");
    hk_entry_comp();
}

void HYPKERNEL32 hk_main(multiboot_info_t const * const multiboot_info)
{
    //init text_position
    text_pos = 0;
    //detect architecture
    hk_print_str("*Checking architecture...\n");
    if (hk_support_x64() == 0)
    {
        hk_print_str("Arch: x86.\n\n");
        hk_init_x86(multiboot_info);
    }
    else
    {
        hk_print_str("Arch: x86_64.\n\n");
        hk_init_x64(multiboot_info);
    }
    return;
}



