#include <common/cdef.h>
#include <common/consts.h>
#include <ke/print.h>
#include <arch/print.h>
#include <arch/brute.h>
#include <arch/mlayout.h>
#include <arch/pmap.h>
#include <mm/phys.h>

#include "multiboot2.h"

// kernel entry point
extern void
kmain();

/*
 * process muliboot info and populate various subsystems
 * after this mate, the original multiboot info can be safely discarded
 */
static void
proc_mbinfo(void *mb_info)
{
    uint8 mmap_detected = 0;

    for (struct multiboot_tag *tag = (struct multiboot_tag *) ((uintptr) mb_info + 8);
         tag->type != MULTIBOOT_TAG_TYPE_END;
         tag = (struct multiboot_tag *) ((multiboot_uint8_t *) tag
                                         + ((tag->size + 7) & ~7u))) {
        PDBG("Tag 0x%p: %d, Size %d", (void *) tag, tag->type, tag->size);
        switch (tag->type) {
            case MULTIBOOT_TAG_TYPE_MMAP:
                mmap_detected = 1;
                PDBG("Detected MultiBoot memory map.");
                for (struct multiboot_mmap_entry *entry = ((struct multiboot_tag_mmap *) tag)->entries;
                     (multiboot_uint8_t *) entry < (multiboot_uint8_t *) tag + tag->size;
                     entry = (multiboot_memory_map_t *) ((uintptr) entry +
                                                         ((struct multiboot_tag_mmap *) tag)->entry_size)) {

                    PDBG("Adding to pmap seg: base = 0x%lx,"
                         " length = 0x%lx, type = 0x%x.",
                         (ulong) entry->addr,
                         (ulong) entry->len,
                         entry->type);

                    if (entry->type == MULTIBOOT_MEMORY_AVAILABLE) {
                        // add physical segments to mm phys subsystem
                        archp_mem_addseg(entry->addr, entry->len);
                    }
                }
                break;
            case MULTIBOOT_TAG_TYPE_BOOT_LOADER_NAME:
                PDBG("Detected bootloader: %s", ((struct multiboot_tag_string *) tag)->string);
                break;
            default:
                PDBG("Ignoring MultiBoot tag type: %d size: 0x%x", tag->type, tag->size);
                break;
        }
    }

    if (!mmap_detected) {
        BRUTE("proc_mbinfo: could not find MMAP tag.");
    }
}

ATTR_USED void
arch_main(void *mb_info)
{
    arch_print_init();

    PINFO("BoND %s. Kernel loaded at: 0x%p size: 0x%lx.", KVERSION, (void *) ARCH_ML_KIMAGE_START,
          (usize) (ARCH_ML_KIMAGE_STOP - ARCH_ML_KIMAGE_START));

    PINFO("Processing MultiBoot struct at 0x%p...", mb_info);
    proc_mbinfo(mb_info);
    // at this point we don't need access to multiboot info anymore -> we can freely overwrite this region

    PINFO("Initializing early memory...");
    archp_mem_init();

    kmain();
}
