/* Copyright 2016 secXsQuared
 * Distributed under GPL license
 * See COPYING under root for details
 */

#ifndef _LINKED_LIST_H_
#define _LINKED_LIST_H_

#include "k_def.h"

typedef struct _linked_list_node_t
{
    struct _linked_list_node_t *prev;
    struct _linked_list_node_t *next;
} linked_list_node_t;

typedef struct _linked_list_t
{
    linked_list_node_t *head;
    linked_list_node_t *tail;
} linked_list_t;

void SAPI linked_list_init(linked_list_t *list);

int SAPI linked_list_size(linked_list_t *list);

void SAPI linked_list_push_front(linked_list_t *list, linked_list_node_t *node);

void SAPI linked_list_push_back(linked_list_t *list, linked_list_node_t *node);

linked_list_node_t *SAPI linked_list_pop_front(linked_list_t *list);

linked_list_node_t *SAPI linked_list_pop_back(linked_list_t *list);

void SAPI linked_list_insert_idx(linked_list_t *list, int32_t index, linked_list_node_t *node);

void SAPI linked_list_insert_ref(linked_list_t *list, linked_list_node_t *prev_node, linked_list_node_t *node);

linked_list_node_t *SAPI linked_list_remove_idx(linked_list_t *list, int32_t index);

linked_list_node_t *SAPI linked_list_remove_ref(linked_list_t *list, linked_list_node_t *node);

linked_list_node_t *SAPI linked_list_get(linked_list_t *list, int32_t index);

linked_list_node_t *SAPI linked_list_next(linked_list_node_t *node);

linked_list_node_t *SAPI linked_list_prev(linked_list_node_t *node);

linked_list_node_t *SAPI linked_list_first(linked_list_t *list);

linked_list_node_t *SAPI linked_list_last(linked_list_t *list);

int32_t SAPI linked_list_search(linked_list_t *list, linked_list_node_t* target, bool (*equals)(linked_list_node_t*, linked_list_node_t*));

#endif