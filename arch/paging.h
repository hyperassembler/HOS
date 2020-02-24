#pragma once

#include <common/libkern.h>
#include <arch/mlayout.h>

typedef uint64_t arch_pml4e;
typedef uint64_t arch_pdpte;
typedef uint64_t arch_pde;
typedef uint64_t arch_pte;

/**
 Page Table Definitions
**/

#define PML4E_ATTR_P (1ul << 0u)
#define PML4E_ATTR_RW (1ul << 1u)
#define PML4E_ATTR_US (1ul << 2u)
#define PML4E_ATTR_PWT (1ul << 3u)
#define PML4E_ATTR_PCD (1ul << 4u)
#define PML4E_ATTR_A (1ul << 5u)
#define PML4E_ATTR_NX (1ul << 63u)

#define PDPTE_ATTR_P (1ul << 0u)
#define PDPTE_ATTR_RW (1ul << 1u)
#define PDPTE_ATTR_US (1ul << 2u)
#define PDPTE_ATTR_PWT (1ul << 3u)
#define PDPTE_ATTR_PCD (1ul << 4u)
#define PDPTE_ATTR_A (1ul << 5u)
#define PDPTE_ATTR_D (1ul << 6u)
#define PDPTE_ATTR_PS (1ul << 7u)
#define PDPTE_ATTR_G (1ul << 8u)
#define PDPTE_ATTR_PAT (1ul << 12u)
#define PDPTE_ATTR_NX (1ul << 63u)

#define PDE_ATTR_P (1ul << 0u)
#define PDE_ATTR_RW (1ul << 1u)
#define PDE_ATTR_US (1ul << 2u)
#define PDE_ATTR_PWT (1ul << 3u)
#define PDE_ATTR_PCD (1ul << 4u)
#define PDE_ATTR_A (1ul << 5u)
#define PDE_ATTR_D (1ul << 6u)
#define PDE_ATTR_PS (1ul << 7u)
#define PDE_ATTR_G (1ul << 8u)
#define PDE_ATTR_PAT (1ul << 12u)
#define PDE_ATTR_NX (1ul << 63u)

#define PTE_ATTR_P (1ul << 0u)
#define PTE_ATTR_RW (1ul << 1u)
#define PTE_ATTR_US (1ul << 2u)
#define PTE_ATTR_PWT (1ul << 3u)
#define PTE_ATTR_PCD (1ul << 4u)
#define PTE_ATTR_A (1ul << 5u)
#define PTE_ATTR_D (1ul << 6u)
#define PTE_ATTR_PS (1ul << 7u)
#define PTE_ATTR_G (1ul << 8u)
#define PTE_ATTR_PAT (1ul << 12u)
#define PTE_ATTR_NX (1ul << 63u)

#define PML4_ENTRY_NUM(vaddr) (((vaddr) >> 39u) & 0x1FFu)
#define PDPT_ENTRY_NUM(vaddr) (((vaddr) >> 30u) & 0x1FFu)
#define PD_ENTRY_NUM(vaddr)   (((vaddr) >> 21u) & 0x1FFu)
#define PT_ENTRY_NUM(vaddr)   (((vaddr) >> 12u) & 0x1FFu)

#define PDPTE_MAPPING_SZ (PDE_MAPPING_SZ * 512)
#define PDE_MAPPING_SZ (PTE_MAPPING_SZ * 512)
#define PTE_MAPPING_SZ (ARCH_KPAGE_SZ)

static inline void
arch_write_page_entry(uint64_t *base, mm_paddr offset, uint64_t attr)
{
    attr = (offset & 0xFFFFFFFFFF000ul) | attr;
    memcpy(base, &attr, sizeof(uint64_t));
}

// trace the page table to see if there exists a pdpte entry for a given pml4
// note _nr means this doesn't depend on the recursive page mapping
static inline int
arch_pdpte_exists_nr(arch_pml4e *pml4, mm_paddr paddr)
{

}

static inline int
arch_pde_exists_nr()
{

}

static inline int
arch_pte_exists_nr()
{

}
