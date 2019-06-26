#pragma once

#include <kern/cdef.h>

void
arch_write_page_tbl(void *base, uintptr pdpt_addr, uint64 attr);

void*
arch_pmap_map(uintptr paddr, usize size);

