#ifndef _LIB_LINKED_LIST_H_
#define _LIB_LINKED_LIST_H_

#include "type.h"

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

/*
 * Returns true if current list node == your node
 * false otherwise
 */

void SXAPI lb_linked_list_init(linked_list_t *list);

int32_t SXAPI lb_linked_list_size(linked_list_t *list);

void SXAPI lb_linked_list_push_front(linked_list_t *list, linked_list_node_t *node);

void SXAPI lb_linked_list_push_back(linked_list_t *list, linked_list_node_t *node);

linked_list_node_t *SXAPI lb_linked_list_pop_front(linked_list_t *list);

linked_list_node_t *SXAPI lb_linked_list_pop_back(linked_list_t *list);

void SXAPI lb_linked_list_insert(linked_list_t *list, int32_t index, linked_list_node_t *node);

void SXAPI lb_linked_list_insert_ref(linked_list_t *list, linked_list_node_t *prev_node, linked_list_node_t *node);

linked_list_node_t *SXAPI lb_linked_list_remove(linked_list_t *list, int32_t index);

linked_list_node_t *SXAPI lb_linked_list_remove_ref(linked_list_t *list, linked_list_node_t *node);

linked_list_node_t *SXAPI lb_linked_list_get(linked_list_t *list, int32_t index);

linked_list_node_t *SXAPI lb_linked_list_next(linked_list_node_t *node);

linked_list_node_t *SXAPI lb_linked_list_prev(linked_list_node_t *node);

linked_list_node_t *SXAPI lb_linked_list_first(linked_list_t *list);

linked_list_node_t *SXAPI lb_linked_list_last(linked_list_t *list);

int32_t SXAPI lb_linked_list_search(linked_list_t *list, linked_list_node_t *target,
                                   callback_func_t equals);

#endif