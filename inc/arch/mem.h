#pragma once

#include <ke/cdef.h>
#include <mm/mm.h>

void
arch_write_page_tbl(void *base, uintptr pdpt_addr, uint64 attr);

void*
arch_pmap_map(phys_addr paddr, usize size);

