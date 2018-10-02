#pragma once

#include "cdef.h"
#include "status.h"
#include "kernel.h"
#include "mlayout.h"

/**
 * physical page allocation
 */
k_status
mm_alloc_page(uintptr *out);

k_status
mm_free_page(uintptr base);

k_status
mm_query_page_attr(uintptr base, int32 *out);

k_status
mm_pmm_init(struct boot_info *info);
