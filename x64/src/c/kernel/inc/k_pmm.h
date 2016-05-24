/* Copyright 2016 secXsQuared
 * Distributed under GPL license
 * See COPYING under root for details
 */

#ifndef _K_PMM_H_
#define _K_PMM_H_

#include "linked_list.h"

void pmm_init(linked_list_t* occupied, linked_list_t* available);

void*pmm_alloc_page();

void pmm_free_page(void *p_addr);

#endif
