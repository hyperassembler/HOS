#ifndef _SYS_INFO_H_
#define _SYS_INFO_H_
#include "type.h"
#include "../util/list/linked_list/linked_list.h"

#define MEMORY_OCCUPIED 0
#define MEMORY_AVAILABLE 1
#define MEMORY_RESERVED 2

typedef struct
{
    uint64_t base_addr;
    uint64_t size;
    linked_list_node_t list_node;
} memory_descriptor_node_t;

typedef struct {
    linked_list_t* mem_seg_list;     // a list containing memory_descriptor_node_t nodes
    uint64_t mem_reserved;
    uint64_t mem_available;
} mem_info_t;

typedef struct
{
    uint64_t base_addr;
    uint64_t size;
    char* name;
    linked_list_node_t list_node;
} module_descriptor_node_t;

typedef struct {
    linked_list_t* module_list;
    uint64_t module_count;
} module_info_t;

typedef struct {
    mem_info_t* mem_info;    // all available memory info
    module_info_t* module_info; // all kernel modules loaded
} boot_info_t;

#endif