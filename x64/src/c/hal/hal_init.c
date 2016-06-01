/* Copyright 2016 secXsQuared
 * Distributed under GPL license
 * See COPYING under root for details
 */

#include "hal_multiboot.h"
#include "hal_print.h"
#include "hal_mem.h"
#include "hal_intr.h"
#include "hal_var.h"
#include "k_sys_info.h"
#include "std_lib.h"

boot_info_t *KAPI hal_init(void *_m_info)
{
    multiboot_info_t* m_info = (multiboot_info_t*) _m_info;
    if (m_info == NULL)
        return NULL;

    // set up kernel heap;
    hal_alloc_init();

    boot_info_t *boot_info = (boot_info_t *) halloc(sizeof(boot_info_t));
    text_pos = get_pos(0, 0);

    // get gdt ready
    hal_write_segment_descriptor((void *) &g_gdt[0], 0, 0, 0);
    hal_write_segment_descriptor((void *) &g_gdt[8], 0, 0,
                                 SEG_DPL_0 | SEG_CODE_DATA | SEG_PRESENT | SEG_LONG | SEG_TYPE_CODE_X);
    hal_write_segment_descriptor((void *) &g_gdt[16], 0, 0,
                                 SEG_DPL_0 | SEG_CODE_DATA | SEG_PRESENT | SEG_LONG | SEG_TYPE_DATA_RW);
    hal_write_segment_descriptor((void *) &g_gdt[24], 0, 0,
                                 SEG_DPL_3 | SEG_CODE_DATA | SEG_PRESENT | SEG_LONG | SEG_TYPE_CODE_X);
    hal_write_segment_descriptor((void *) &g_gdt[32], 0, 0,
                                 SEG_DPL_3 | SEG_CODE_DATA | SEG_PRESENT | SEG_LONG | SEG_TYPE_DATA_RW);

    hal_write_segment_descriptor((void *) &g_gdt[40], 0, 0xFFFFF,
                                 SEG_DPL_0 | SEG_GRANULARITY | SEG_CODE_DATA | SEG_PRESENT | SEG_32_BITS |
                                 SEG_TYPE_CODE_X);
    hal_write_segment_descriptor((void *) &g_gdt[48], 0, 0xFFFFF,
                                 SEG_DPL_0 | SEG_GRANULARITY | SEG_CODE_DATA | SEG_PRESENT | SEG_32_BITS |
                                 SEG_TYPE_DATA_RW);
    hal_write_segment_descriptor((void *) &g_gdt[56], 0, 0xFFFFF,
                                 SEG_DPL_3 | SEG_GRANULARITY | SEG_CODE_DATA | SEG_PRESENT | SEG_32_BITS |
                                 SEG_TYPE_CODE_X);
    hal_write_segment_descriptor((void *) &g_gdt[64], 0, 0xFFFFF,
                                 SEG_DPL_3 | SEG_GRANULARITY | SEG_CODE_DATA | SEG_PRESENT | SEG_32_BITS |
                                 SEG_TYPE_DATA_RW);
    g_gdt_ptr.base = (uint64_t) g_gdt;
    g_gdt_ptr.limit = 8 * 9 - 1;
    hal_flush_gdt(&g_gdt_ptr, seg_selector(1, 0), seg_selector(2, 0));

    // get idt ptr ready
    g_idt_ptr.base = (uint64_t) g_idt;
    g_idt_ptr.limit = 21 * 16 - 1;
    hal_flush_idt(&g_idt_ptr);

    mem_set(boot_info, 0, sizeof(boot_info_t));
    // obtain boot information
    // memory info
    if (m_info->flags & (1 << 6))
    {
        boot_info->mem_info = (mem_info_t *) halloc(sizeof(mem_info_t));
        hal_assert(boot_info->mem_info != NULL, "Unable to allocate memory for mem_info.");
        boot_info->mem_info->mem_available = 0;
        boot_info->mem_info->mem_installed = 0;
        boot_info->mem_info->free_page_list = (linked_list_t *) halloc((sizeof(linked_list_t)));
        boot_info->mem_info->occupied_page_list = (linked_list_t *) halloc((sizeof(linked_list_t)));
        hal_assert(boot_info->mem_info->free_page_list != NULL &&
                   boot_info->mem_info->occupied_page_list != NULL, "Unable to allocate memory for mem_info_lists.");
        linked_list_init(boot_info->mem_info->free_page_list);
        linked_list_init(boot_info->mem_info->occupied_page_list);
        multiboot_memory_map_t *mem_map = (multiboot_memory_map_t *) (uint64_t)m_info->mmap_addr;
        uint64_t mem_map_size = m_info->mmap_length / sizeof(multiboot_memory_map_t);
        for (uint64_t i = 0; i < mem_map_size; i++)
        {
            hal_printf("==Base: 0x%X, Length: %u, Type: %s==\n", (mem_map + i)->addr, (mem_map + i)->len,
                       (mem_map + i)->type == MULTIBOOT_MEMORY_AVAILABLE ? "AVL" : "RSV");
            if ((mem_map + i)->type == MULTIBOOT_MEMORY_AVAILABLE)
            {
                uint64_t base_addr = (mem_map + i)->addr;
                uint64_t end_addr = base_addr + (mem_map + i)->len;

                // align head
                uint64_t aligned_base_addr = align_up(base_addr, PHYSICAL_PAGE_SIZE);
                // align tail
                uint64_t aligned_end_addr = align_down(end_addr, PHYSICAL_PAGE_SIZE);


                uint64_t page_count = (aligned_end_addr - aligned_base_addr) / PHYSICAL_PAGE_SIZE;

                if (page_count == 0)
                    continue;

                // strip kernel-occupied pages
                // TODO: Finished this.
//                uint64_t aligned_kernel_base = ALIGN_DOWN((uint64_t)kernel_start, PHYSICAL_PAGE_SIZE);
//                uint64_t aligned_kernel_end = ALIGN_UP((uint64_t)kernel_end, PHYSICAL_PAGE_SIZE);
//                if(IS_OVERLAP(aligned_base_addr,aligned_end_addr,aligned_kernel_base,aligned_kernel_end))
//                {
//                    uint64_t overlap_pages = (MIN(aligned_kernel_end, aligned_end_addr)
//                                        - MAX(aligned_kernel_base, aligned_base_addr)) / PHYSICAL_PAGE_SIZE;
//
//                    if(overlap_pages != 0)
//                    {
//                        // if there is overlap, add to occupied list
//                        memory_descriptor_node_t *occupied_desc = (memory_descriptor_node_t *) hal_alloc(
//                                sizeof(memory_descriptor_node_t));
//                        hal_assert(occupied_desc != NULL, "Unable to allocate memory for memory_descriptor.");
//                        occupied_desc->base_addr = aligned_kernel_base;
//                        occupied_desc->page_count = overlap_pages;
//                        linked_list_add(boot_info->mem_info->occupied_page_list, &occupied_desc->list_node);
//
//                        // also adjust corresponding segment
//                        page_count = page_count - overlap_pages;
//                        if(page_count == 0) // if kernel occupies the whole segment, continue
//                            continue;
//
//                    }
//                }

                memory_descriptor_node_t *each_desc = (memory_descriptor_node_t *) halloc(
                        sizeof(memory_descriptor_node_t));
                hal_assert(each_desc != NULL, "Unable to allocate memory for memory_descriptor.");
                each_desc->page_count = page_count;
                each_desc->base_addr = aligned_base_addr;
                linked_list_push_back(boot_info->mem_info->free_page_list, &each_desc->list_node);
                boot_info->mem_info->mem_available += aligned_end_addr - aligned_base_addr;
            }
            boot_info->mem_info->mem_installed += (mem_map + i)->len;
        }
    }
    else
    {
        // halt machine
        hal_printf("HAL: Cannot detect memory information.");
        hal_halt_cpu();
    }

    // loaded kernel modules
    if (m_info->flags & (1 << 3))
    {
        boot_info->module_info = (module_info_t *) halloc(sizeof(module_info_t));
        hal_assert(boot_info->module_info != NULL, "Unable to allocate memory for module_info.");
        boot_info->module_info->module_count = 0;
        boot_info->module_info->module_list = (linked_list_t *) halloc(sizeof(linked_list_t));
        hal_assert(boot_info->module_info->module_list != NULL, "Unable to allocate memory for module_list.");
        linked_list_init(boot_info->module_info->module_list);
        multiboot_module_t *mods_list = (multiboot_module_t *) (uint64_t) m_info->mods_addr;
        boot_info->module_info->module_count = m_info->mods_count;
        for (uint64_t i = 0; i < boot_info->module_info->module_count; i++)
        {
            module_descriptor_node_t *each_module = (module_descriptor_node_t *) halloc(
                    sizeof(module_descriptor_node_t));
            hal_assert(each_module != NULL, "Unable to allocate memory for module_descriptor.");
            each_module->base_addr = (mods_list + i)->mod_start;
            each_module->size = (mods_list + i)->mod_end - (mods_list + i)->mod_start;
            each_module->name = (char *) halloc((size_t) str_len((char *) (uint64_t) (mods_list + i)->cmdline) + 1);
            hal_assert(each_module->name != NULL, "Unable to allocate memory for module name string.");
            mem_cpy((void *) (uint64_t) (mods_list + i)->cmdline, each_module->name,
                    str_len((char *) (uint64_t) (mods_list + i)->cmdline) + 1);
            linked_list_push_back(boot_info->module_info->module_list, &each_module->list_node);
        }
    }

    // detect APIC
    cpuid_t cpuid_info;
    cpuid_info.eax = 1;
    cpuid_info.ebx = 0;
    cpuid_info.ecx = 0;
    cpuid_info.edx = 0;
    hal_cpuid(&cpuid_info.eax, &cpuid_info.ebx, &cpuid_info.ecx, &cpuid_info.edx);
    if (cpuid_info.edx & 1 << 9)
    {
        //TODO: detected.
    }
    else
    {
        // halt machine
        hal_printf("HAL: Cannot detect APIC.");
        hal_halt_cpu();
    }

    return boot_info;
}
