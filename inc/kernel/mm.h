#pragma once

#include "common.h"
#include "kernel/status.h"
#include "kernel/lb.h"
#include "kernel/ke.h"

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
