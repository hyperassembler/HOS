#pragma once

#include "kernel/mm.h"

/**
 * PMM init info
 */
struct pmm_node
{
    uintptr base;
    uint64 size;
    uint32 attr;
};

struct pmm_info
{
    uint32 num_of_nodes;
    struct pmm_node nodes[];
};

