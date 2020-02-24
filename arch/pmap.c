#include <arch/pmap.h>
#include <ke/brute.h>
#include <mm/phys.h>
#include <common/libkern.h>

#include "pmap_p.h"
#include "paging.h"

struct arch_pmap_segs {
    mm_paddr start;
    mm_paddr stop;
};

// the physical memory segments information obtained from multiboot info
static struct arch_pmap_segs _phys_segs[ARCH_PMAP_MAX_PHYS_SEGS];
static usize _phys_segs_sz = 0;

// the base addr for mm_page structures
static mm_paddr _mm_pages_base;

// initializes _pmap region
static void
_pmap_init(mm_paddr *cur_addr, arch_pml4e *kern_pml4)
{
    usize high_mem = _phys_segs[_phys_segs_sz - 1].stop;

    if (high_mem >= ARCH_ML_MAX_RAM) {
        BRUTE("Only supports maximum %ld bytes RAM", ARCH_ML_MAX_RAM);
    }

    kprintf("Total memory size: %ld bytes", high_mem + 1);

    // map all 1GB sections
    usize num = ((high_mem + 1) & ~0x3FFFFFFFu) >> 30u;
    for (usize i = 0; i < num; i++) {
        kprintf("");
    }

    kprintf("pmap: 1GB segment");

    // map all 2MB sections

    // map all 4KB sections
}

static void
_mm_pg_init(mm_paddr *cur_addr, arch_pml4e *kern_pml4)
{

}

// initializes kernel mapping
static void
_kern_mapping_init(mm_paddr *cur_addr, arch_pml4e *kern_pml4)
{
    // map the kernel with 2MB mapping now
    KASSERT((ARCH_ML_KIMAGE_PADDR & (PDE_MAPPING_SZ - 1)) == 0, "kernel vaddr not 2MB aligned.");
    KASSERT(((uintptr) KERN_IMG_START & (PDE_MAPPING_SZ - 1)) == 0, "kernel paddr not 2MB aligned.");

    const uintptr kern_map_end = ALIGN_UP2((uintptr) (ARCH_ML_KIMAGE_STOP), PDE_MAPPING_SZ);

    kprintf("kern_map_end: 0x%p", (void *) kern_map_end);

    *cur_addr = kern_map_end - KERN_BASE_START;
    *kern_pml4 = arch_pmap_map(ARCH_ML_PMAP_START + *cur_addr, ARCH_KPAGE_SZ);
    *cur_addr += ARCH_KPAGE_SZ;

    // pdpt for the kernel, kernel must be within the first GB so only 1
    arch_pdpte *kern_pdpt = cur_addr;
    cur_addr += ARCH_KPAGE_SZ * kern_num_pdpt;

    arch_write_page_entry(kern_pml4 + PML4_ENTRY_NUM((uintptr) KERN_IMG_START),
                          kern_pdpt, PML4E_ATTR_P | PML4E_ATTR_RW);
    // pd for the kernel
    const uintptr kern_pd = cur_addr;
    cur_addr += ARCH_KPAGE_SZ;

    kern_pml4 = cur_addr;
    cur_addr += ARCH_KPAGE_SZ;
}

// maps device memory
void *
arch_pmap_mapdev(ATTR_UNUSED uintptr paddr, ATTR_UNUSED usize size)
{
    return NULL;
}

// maps a physical segment to pmap region
void *
arch_pmap_map(mm_paddr paddr, ATTR_UNUSED usize sz)
{
    return (void *) ARCH_PHYS_TO_PMAP(paddr);
}

// obtains the paddr of the corresponding struct mm_page for a specific paddr
mm_paddr
arch_paddr_to_mm_page(mm_paddr paddr)
{
    return (paddr / ARCH_KPAGE_SZ) * sizeof(struct mm_page) + _mm_pages_base;
}

// adds an available physical segment to _phys_segs
void
arch_mem_addseg(mm_paddr start, usize len)
{
    KASSERT(_phys_segs_sz < ARCH_PMAP_MAX_PHYS_SEGS, "too many physical segments!");
    _phys_segs[_phys_segs_sz].start = start;
    _phys_segs[_phys_segs_sz].stop = start + len - 1;
    _phys_segs_sz++;
}

// sets up paging for kernel and mm_pages for mm_phys
// specifically, we do:
// 1. Map the kernel to KERN_BASE
// 2. Allocate mm_page for the all physical memory (avail and unavail) and put them after the kernel paddr
// 2.5. Map all mm_page (s) to KERN_MM_PAGE_START
// 3. Direct map all available physical memory to PMAP_BASE
// 4. Save the mapping and switch the page table to the new table
// 5. Save the information to mm_phys for future phys setup
void
arch_mem_init()
{
    // we use 2M (PDE) pages to map the kernel so align the physical address to 2MB
    mm_paddr cur_addr = ALIGN_UP2(ARCH_ML_KIMAGE_PADDR, PDE_MAPPING_SZ);

    // allocate the pml4 for the kernel
    arch_pml4e *kern_pml4 = (arch_pml4e *) ARCH_PHYS_TO_PMAP(cur_addr);
    memset(kern_pml4, 0, ARCH_KPAGE_SZ);
    cur_addr += ARCH_KPAGE_SZ;

    _kern_mapping_init(&cur_addr, kern_pml4);
    _mm_pg_init(&cur_addr, kern_pml4);
    _pmap_init(&cur_addr, kern_pml4);

    // copy the physical segments information to mm_phys
    for (usize i = 0; i < _phys_segs_sz; i++) {
        mm_phys_add_phys_seg(_phys_segs[i].start, _phys_segs[i].stop);
    }

    // add reserved segment information to mm_phys
    // we reserve everything from KERN_IMG_PADDR to what we've allocated so far
    mm_phys_add_reserved_seg(ARCH_ML_KIMAGE_PADDR, cur_addr - ARCH_ML_KIMAGE_PADDR + 1);
    // reserve the 0th page
    mm_phys_add_reserved_seg(0, ARCH_KPAGE_SZ);
}
