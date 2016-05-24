/* Copyright 2016 secXsQuared
 * Distributed under GPL license
 * See COPYING under root for details
 */

#include "k_pmm.h"
#include "k_sys_info.h"
#include "hal_io.h"
#include "std_lib.h"

linked_list_t* occupied_mem;
linked_list_t* available_mem;

void pmm_init(linked_list_t* occupied, linked_list_t* available)
{
    hal_assert(occupied_mem != NULL && available_mem != NULL, NULL);
    occupied_mem = occupied;
    available_mem = available;
    return;
}

void _pmm_add_page(linked_list_t* list,uint64_t base_addr, uint64_t size)
{
    hal_assert(list != NULL,NULL);
    base_addr = (base_addr >> 12) << 12;
    if(linked_list_size(list) == 0)
    {
        return;
    }
    else
    {
        for (int i = 0; i < linked_list_size(list); i++) {
            memory_descriptor_node_t *each_node = OBTAIN_STRUCT_ADDR(linked_list_get(list, i), list_node,
                                                                     memory_descriptor_node_t);

        }
    }
}

void* pmm_alloc_page()
{
    hal_assert(occupied_mem != NULL && available_mem != NULL, NULL);
    if(linked_list_size(available_mem) == 0)
        return NULL;
    for(int i = 0; i < linked_list_size(available_mem); i++)
    {
//        memory_descriptor_node_t* each_node = OBTAIN_STRUCT_ADDR(linked_list_get(available_mem,i),list_node,memory_descriptor_node_t);
//        if(each_node->size >= PHYSICAL_PAGE_SIZE)
//        {
//            uint64_t address = each_node->base_addr;
//            // found, add to occupied list and return
//            each_node->size = each_node->size - PHYSICAL_PAGE_SIZE;
//            each_node->base_addr = each_node->base_addr + PHYSICAL_PAGE_SIZE;
//
//
//            return (void*)address;
//        }
    }

    // not found
    return NULL;
}

void pmm_free_page(void *physical_addr)
{

}
