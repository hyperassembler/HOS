#ifndef _PMM_H_
#define _PMM_H_

#include "../common/util/list/linked_list/linked_list.h"

void pmm_init(linked_list_t* occupied, linked_list_t* available);

void*pmm_alloc_page();

void pmm_free_page(void *p_addr);

#endif
