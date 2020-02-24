#include <common/cdef.h>
#include <ke/print.h>
#include <arch/print.h>
#include <arch/brute.h>
#include <mm/phys.h>
#include <arch/mlayout.h>

// private headers
#include "multiboot2.h"
#include "pmap_p.h"

static const char* _loader_name;

// kernel entry point
extern void kmain();

ATTR_USED void
arch_main(void *mb_info)
{
    /* init printf related stuff */
    arch_print_init();

    kprintf("Processing multiboot info @ 0x%p...\n", mb_info);

    for (struct multiboot_tag *tag = (struct multiboot_tag *) ((uintptr) mb_info + 8);
         tag->type != MULTIBOOT_TAG_TYPE_END;
         tag = (struct multiboot_tag *) ((multiboot_uint8_t *) tag
                                         + ((tag->size + 7) & ~7u))) {
//        kprintf("Tag 0x%p: %d, Size %d\n", (void *) tag, tag->type, tag->size);
        switch (tag->type) {
            case MULTIBOOT_TAG_TYPE_MMAP:
                kprintf("Found multiboot memory map.\n");
                for (struct multiboot_mmap_entry *entry = ((struct multiboot_tag_mmap *) tag)->entries;
                     (multiboot_uint8_t *) entry < (multiboot_uint8_t *) tag + tag->size;
                     entry = (multiboot_memory_map_t *) ((uintptr) entry +
                                                         ((struct multiboot_tag_mmap *) tag)->entry_size)) {
                    kprintf("Adding to pmap seg: base = 0x%lx,"
                            " length = 0x%lx, type = 0x%x.\n",
                            (ulong) entry->addr,
                            (ulong) entry->len,
                            entry->type);
                    if (entry->type == MULTIBOOT_MEMORY_AVAILABLE) {
                        // add physical segments to mm phys subsystem
                        arch_mem_addseg(entry->addr, entry->len);
                    }
                }
                break;
            case MULTIBOOT_TAG_TYPE_BOOT_LOADER_NAME:
                kprintf("Found multiboot loader name.\n");
                _loader_name = ((struct multiboot_tag_string *) tag)->string;
                break;
            default:
                kprintf("Ignoring multiboot tag type: %d size: 0x%x\n", tag->type, tag->size);
                break;
        }
    }

    kprintf("BoND is loaded by: %s\n", _loader_name);
    kprintf("kernel start: 0x%p end: 0x%p\n", (void*)KERN_IMG_START, (void*)ARCH_ML_KIMAGE_STOP);

    kprintf("Initializing memory...");
    arch_mem_init();

    kmain();
}
