#include <mm/phys.h>
#include <common/libkern.h>
#include <arch/pmap.h>
#include <ke/status.h>
#include "paging.h"

mm_paddr page_alloc_zero()
{
    return 0;
}

/* map physical memory to virtual */
// XXX: this should undo stuff if pages are not enough
// TODO: use huge pages later (2MB + 1GB)
static int
_map_page(arch_pml4e *pml4, mm_paddr paddr, mm_paddr vaddr, uint attr)
{
    int uspace = vaddr < ARCH_ML_KSPACE_START;

    if (uspace && (vaddr > ARCH_ML_USPACE_END)) {
        BRUTE("non-canonical vaddr");
    }

    /* must be 4k aligned */
    if((paddr & (ARCH_KPAGE_SZ - 1)) != 0 ||
       (vaddr & (ARCH_KPAGE_SZ - 1)) != 0 ){
        BRUTE("addresses not aligned");
    }

    mm_paddr alloc_pt;

    arch_pml4e *pml4_ent = pml4 + PML4_ENTRY_NUM(vaddr);
    if (*pml4_ent == 0) {
        alloc_pt = page_alloc_zero();
        if (!alloc_pt) {
            BRUTE("not enough pages");
        }
        arch_write_page_entry(pml4_ent, alloc_pt, PML4E_ATTR_RW | PML4E_ATTR_P | (uspace ? PML4E_ATTR_US : 0));
    }

    arch_pdpte *pdpt_ent = (arch_pdpte*)arch_pmap_map(*pml4_ent, ARCH_KPAGE_SZ) + PDPT_ENTRY_NUM(vaddr);

    if (*pdpt_ent == 0) {
        alloc_pt = page_alloc_zero();
        if (!alloc_pt) {
            BRUTE("not enough pages");
        }
        arch_write_page_entry(pdpt_ent, alloc_pt, PDPTE_ATTR_RW | PDPTE_ATTR_P | (uspace ? PDPTE_ATTR_US : 0));
    }

    arch_pde *pde_ent = (arch_pdpte*)arch_pmap_map(*pdpt_ent, ARCH_KPAGE_SZ) + PD_ENTRY_NUM(vaddr);

    if (*pde_ent == 0) {
        alloc_pt = page_alloc_zero();
        if (!alloc_pt) {
            BRUTE("not enough pages");
        }
        arch_write_page_entry(pde_ent, alloc_pt, PDE_ATTR_RW | PDE_ATTR_P | (uspace ? PDE_ATTR_US : 0));
    }

    arch_pte *pte_ent = (arch_pdpte*)arch_pmap_map(*pde_ent, ARCH_KPAGE_SZ) + PT_ENTRY_NUM(vaddr);

    if (*pte_ent != 0) {
        BRUTE("vaddr 0x%p is already mapped", (void*)vaddr);
    }

    uint64 pattr = 0;

    pattr |= (attr & ARCH_VADDR_ATTR_PRESENT ? PTE_ATTR_P : 0);
    pattr |= (attr & ARCH_VADDR_ATTR_NX ? PTE_ATTR_NX : 0);
    pattr |= (attr & ARCH_VADDR_ATTR_READONLY ? 0 : PTE_ATTR_RW);
    pattr |= (attr & ARCH_VADDR_ATTR_UNCACHED ? PTE_ATTR_PCD : 0);

    arch_write_page_entry(pte_ent, paddr, pattr);

    return S_OK;
}

int
arch_map_vaddr(void * base, mm_paddr paddr, uintptr vaddr, usize sz, uint attr)
{
    if ((sz & (ARCH_KPAGE_SZ - 1)) != 0) {
        BRUTE("Unaligned size");
    }

    for(mm_paddr caddr = paddr; caddr < paddr + sz; caddr += ARCH_KPAGE_SZ) {
        _map_page(base, paddr, vaddr, attr);
    }

    return S_OK;
}
