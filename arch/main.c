#include <kern/cdef.h>
#include <kern/print.h>
#include <arch/print.h>
#include <arch/brute.h>

// private headers
#include "multiboot2.h"

ATTR_USED void
arch_main(void *mb_info)
{
    /* init printf related stuff */
    arch_print_init();

    kprintf("Multiboot info: 0x%p\n", mb_info);
    kprintf("Initializing arch layer...\n");

    for (struct multiboot_tag *tag = (struct multiboot_tag *) ((uintptr) mb_info + 8);
         tag->type != MULTIBOOT_TAG_TYPE_END;
         tag = (struct multiboot_tag *) ((multiboot_uint8_t *) tag
                                         + ((tag->size + 7) & ~7u))) {
        kprintf("Tag 0x%p: %d, Size %d\n", (void *) tag, tag->type, tag->size);
        switch (tag->type) {
            case MULTIBOOT_TAG_TYPE_MMAP:
                for (struct multiboot_mmap_entry *entry = ((struct multiboot_tag_mmap *) tag)->entries;
                     (multiboot_uint8_t *) entry < (multiboot_uint8_t *) tag + tag->size;
                     entry = (multiboot_memory_map_t *) ((uintptr) entry +
                                                         ((struct multiboot_tag_mmap *) tag)->entry_size))
                    kprintf(" base_addr = 0x%lx,"
                            " length = 0x%lx, type = 0x%x\n",
                            (ulong) entry->addr,
                            (ulong) entry->len,
                            entry->type);
                break;
            case MULTIBOOT_TAG_TYPE_BOOT_LOADER_NAME:
                kprintf("BoND is loaded by: %s\n", ((struct multiboot_tag_string *) tag)->string);
                break;
            default:
                kprintf("Unhandled multiboot tag type: %d\n", tag->type);
                break;
        }
    }

    kprintf("Arch layer initialized.\n");

    arch_brute();
}
