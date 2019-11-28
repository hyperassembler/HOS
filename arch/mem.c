#include <kern/cdef.h>
#include <arch/mem.h>
#include <arch/mlayout.h>

/**
 Page Table Definitions
**/

#define PML4_PRESENT (1ull << 0)
#define PML4_WRITE (1ull << 1)
#define PML4_USER (1ull << 2)
#define PML4_WRITE_THROUGH (1ull << 3)
#define PML4_CACHE_DISABLED (1ull << 4)
#define PML4_ACCESSED (1ull << 5)
#define PML4_EXECUTION_DISABLED (1ull << 63)

#define PDPT_PRESENT (1ull << 0)
#define PDPT_WRITE (1ull << 1)
#define PDPT_USER (1ull << 2)
#define PDPT_WRITE_THROUGH (1ull << 3)
#define PDPT_CACHE_DISABLED (1ull << 4)
#define PDPT_ACCESSED (1ull << 5)
#define PDPT_EXECUTION_DISABLED (1ull << 63)

#define PD_PRESENT (1ull << 0)
#define PD_WRITE (1ull << 1)
#define PD_USER (1ull << 2)
#define PD_WRITE_THROUGH (1ull << 3)
#define PD_CACHE_DISABLED (1ull << 4)
#define PD_ACCESSED (1ull << 5)
#define PD_EXECUTION_DISABLED (1ull << 63)

#define PT_PRESENT (1ull << 0)
#define PT_WRITE (1ull << 1)
#define PT_USER (1ull << 2)
#define PT_WRITE_THROUGH (1ull << 3)
#define PT_CACHE_DISABLED (1ull << 4)
#define PT_ACCESSED (1ull << 5)
#define PT_DIRTY (1ull << 6)
#define PT_ATTRIBUTE_TABLE (1ull << 7)
#define PT_GLOBAL (1ull << 8)
#define PT_EXECUTION_DISABLED (1ull << 63)

#define PML4_ENTRY_NUM(vaddr) (((vaddr) >> 39) & 0x1FF)
#define PDPT_ENTRY_NUM(vaddr) (((vaddr) >> 30) & 0x1FF)
#define PD_ENTRY_NUM(vaddr)   (((vaddr) >> 21) & 0x1FF)
#define PT_ENTRY_NUM(vaddr)   (((vaddr) >> 12) & 0x1FF)

void
write_page_tbl(void *base, uintptr pdpt_addr, uint64 attr)
{
    if (base == NULL)
    {
        return;
    }
    uint64 entry = (pdpt_addr & 0xFFFFFFFFFF000ul) | attr;
    ((uint8 *) base)[0] = (uint8) (entry & 0xFFul);
    ((uint8 *) base)[1] = (uint8) ((entry >> 8u) & 0xFFu);
    ((uint8 *) base)[2] = (uint8) ((entry >> 16u) & 0xFFu);
    ((uint8 *) base)[3] = (uint8) ((entry >> 24u) & 0xFFu);
    ((uint8 *) base)[4] = (uint8) ((entry >> 32u) & 0xFFu);
    ((uint8 *) base)[5] = (uint8) ((entry >> 40u) & 0xFFu);
    ((uint8 *) base)[6] = (uint8) ((entry >> 48u) & 0xFFu);
    ((uint8 *) base)[7] = (uint8) ((entry >> 56u) & 0xFFu);
}

