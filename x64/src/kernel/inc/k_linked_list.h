/* Copyright 2016 secXsQuared
 * Distributed under GPL license
 * See COPYING under root for details
 */

#ifndef _K_LINKED_LIST_H_
#define _K_LINKED_LIST_H_

#include "g_abi.h"
#include "g_type.h"

typedef struct _k_linked_list_node_t
{
    struct _k_linked_list_node_t *prev;
    struct _k_linked_list_node_t *next;
} k_linked_list_node_t;

typedef struct _k_linked_list_t
{
    k_linked_list_node_t *head;
    k_linked_list_node_t *tail;
} k_linked_list_t;

/*
 * Returns true if current list node == your node
 * false otherwise
 */

void KAPI ke_linked_list_init(k_linked_list_t *list);

int32_t KAPI ke_linked_list_size(k_linked_list_t *list);

void KAPI ke_linked_list_push_front(k_linked_list_t *list, k_linked_list_node_t *node);

void KAPI ke_linked_list_push_back(k_linked_list_t *list, k_linked_list_node_t *node);

k_linked_list_node_t *KAPI ke_linked_list_pop_front(k_linked_list_t *list);

k_linked_list_node_t *KAPI ke_linked_list_pop_back(k_linked_list_t *list);

void KAPI ke_linked_list_insert(k_linked_list_t *list, int32_t index, k_linked_list_node_t *node);

void KAPI ke_linked_list_insert_ref(k_linked_list_t *list, k_linked_list_node_t *prev_node, k_linked_list_node_t *node);

k_linked_list_node_t *KAPI ke_linked_list_remove(k_linked_list_t *list, int32_t index);

k_linked_list_node_t *KAPI ke_linked_list_remove_ref(k_linked_list_t *list, k_linked_list_node_t *node);

k_linked_list_node_t *KAPI ke_linked_list_get(k_linked_list_t *list, int32_t index);

k_linked_list_node_t *KAPI ke_linked_list_next(k_linked_list_node_t *node);

k_linked_list_node_t *KAPI ke_linked_list_prev(k_linked_list_node_t *node);

k_linked_list_node_t *KAPI ke_linked_list_first(k_linked_list_t *list);

k_linked_list_node_t *KAPI ke_linked_list_last(k_linked_list_t *list);

int32_t KAPI ke_linked_list_search(k_linked_list_t *list, k_linked_list_node_t *target,
                                   k_callback_func_t equals);

#endif