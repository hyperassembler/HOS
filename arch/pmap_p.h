#pragma once

#include <arch/mlayout.h>

#define ARCH_PMAP_TO_PHYS(pmaddr) ((pmaddr) -  ARCH_ML_PMAP_START)
#define ARCH_PHYS_TO_PMAP(paddr) (ARCH_ML_PMAP_START + (paddr))

void
arch_mem_init();

void
arch_mem_addseg(uintptr start, usize len);
