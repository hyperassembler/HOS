/* Copyright 2016 secXsQuared
 * Distributed under GPL license
 * See COPYING under root for details
 */

#include "k_linked_list.h"

static void KAPI _init_linked_list_node(k_linked_list_node_t *node)
{
    if (node != NULL)
    {
        node->next = NULL;
        node->prev = NULL;
    }
    return;
}

static void KAPI _append_node(k_linked_list_node_t *target, k_linked_list_node_t *node)
{
    if(target == NULL || node == NULL)
        return;

    k_linked_list_node_t* next = target->next;
    // update the next node
    if(next != NULL)
    {
        next->prev = node;
    }

    // update the target node
    target->next = node;

    // update the node itself
    node->prev = target;
    node->next = next;

    return;
}

// link target with node, suppose target is in the current list
static void KAPI _prepend_node(k_linked_list_node_t *target, k_linked_list_node_t *node)
{
    if(target == NULL || node == NULL)
        return;

    k_linked_list_node_t* prev = target->prev;
    // update the prev node
    if(prev != NULL)
    {
        prev->next = node;
    }

    // update the target node
    target->prev = node;

    // update the node itself
    node->next = target;
    node->prev = prev;

    return;
}

static void KAPI _unlink_node(k_linked_list_node_t* node)
{
    if(node == NULL)
        return;

    if(node->prev != NULL)
    {
        node->prev->next = node->next;
    }

    if(node->next != NULL)
    {
        node->next->prev = node->prev;
    }

    return;
}

void KAPI ke_linked_list_init(k_linked_list_t *list)
{
    if (list != NULL)
    {
        list->head = NULL;
        list->tail = NULL;
    }
    return;
}

int32_t KAPI ke_linked_list_size(k_linked_list_t *list)
{
    if (list == NULL)
        return -1;
    if (list->head == NULL)
        return 0;

    int32_t size = 1;
    k_linked_list_node_t *cur_node = list->head;
    k_linked_list_node_t *tail = list->tail;
    while ((cur_node != tail) && ((cur_node = cur_node->next) != NULL))
    {
        size++;
    }
    return size;
}

void KAPI ke_linked_list_push_front(k_linked_list_t *list, k_linked_list_node_t *node)
{
    if (list == NULL || node == NULL)
        return;

    _init_linked_list_node(node);

    ke_linked_list_insert_ref(list, NULL, node);

    return;
}

void KAPI ke_linked_list_push_back(k_linked_list_t *list, k_linked_list_node_t *node)
{
    if (list == NULL || node == NULL)
        return;

    _init_linked_list_node(node);

    ke_linked_list_insert_ref(list, list->tail, node);

    return;
}

k_linked_list_node_t *KAPI ke_linked_list_pop_front(k_linked_list_t *list)
{
    if (list == NULL)
        return NULL;
    return ke_linked_list_remove_ref(list, list->head);
}

k_linked_list_node_t *KAPI ke_linked_list_pop_back(k_linked_list_t *list)
{
    if (list == NULL)
        return NULL;

    return ke_linked_list_remove_ref(list, list->tail);
}


void KAPI ke_linked_list_insert_ref(k_linked_list_t *list, k_linked_list_node_t *prev_node, k_linked_list_node_t *node)
{
    if (list == NULL || node == NULL)
        return;
    _init_linked_list_node(node);
    if (prev_node == NULL)
    {
        // if prev_node is NULL, then we are inserting to the head

        // linked node with list->head
        _prepend_node(list->head, node);

        if (list->tail == NULL)
        {
            // if the list is empty, we assign list->tail to node too
            list->tail = node;
        }

        list->head = node;
    }
    else
    {
        // if prev_node is not NULL, we are inserting to the middle or the end

        // linked node with the prev_node
        _append_node(prev_node, node);

        if (node->next == NULL)
        {
            // if it's the end
            list->tail = node;
        }
    }
}

void KAPI ke_linked_list_insert(k_linked_list_t *list, int32_t index, k_linked_list_node_t *node)
{
    if (list == NULL || index < 0 || node == NULL)
        return;
    k_linked_list_node_t *prev_node = ke_linked_list_get(list, index - 1);
    _init_linked_list_node(node);

    if (prev_node == NULL)
    {
        if (index == 0)
        {
            ke_linked_list_insert_ref(list, NULL, node);
        }
    }
    else
    {
        ke_linked_list_insert_ref(list, prev_node, node);
    }

    return;
}

k_linked_list_node_t *KAPI ke_linked_list_remove(k_linked_list_t *list, int32_t index)
{
    if (list == NULL || index < 0)
        return NULL;
    k_linked_list_node_t *cur_node = ke_linked_list_get(list, index);

    if (cur_node == NULL)
        return NULL;

    return ke_linked_list_remove_ref(list, cur_node);
}

k_linked_list_node_t *KAPI ke_linked_list_remove_ref(k_linked_list_t *list, k_linked_list_node_t *node)
{
    if (list == NULL || node == NULL)
        return NULL;

    _unlink_node(node);

    if(node->next == NULL)
    {
        list->tail = node->prev;
    }

    if(node->prev == NULL)
    {
        list->head = node->next;
    }

    _init_linked_list_node(node);

    return node;
}

k_linked_list_node_t *KAPI ke_linked_list_get(k_linked_list_t *list, int32_t index)
{
    if (list == NULL || index < 0 || list->head == NULL)
        return NULL;
    k_linked_list_node_t *cur_node = list->head;
    while (index-- && (cur_node = cur_node->next) != NULL);
    return cur_node;
}

k_linked_list_node_t *KAPI ke_linked_list_next(k_linked_list_node_t *node)
{
    if (node != NULL)
    {
        node = node->next;
    }
    return node;
}

k_linked_list_node_t *KAPI ke_linked_list_prev(k_linked_list_node_t *node)
{
    if (node != NULL)
    {
        node = node->prev;
    }
    return node;
}

k_linked_list_node_t *KAPI ke_linked_list_first(k_linked_list_t *list)
{
    k_linked_list_node_t *result = NULL;
    if (list != NULL)
    {
        result = list->head;
    }
    return result;
}

k_linked_list_node_t *KAPI ke_linked_list_last(k_linked_list_t *list)
{
    k_linked_list_node_t *result = NULL;
    if (list != NULL)
    {
        result = list->tail;
    }
    return result;
}

int32_t KAPI ke_linked_list_search(k_linked_list_t *list, k_linked_list_node_t *target,
                                   k_callback_func_t equals)
{
    if(list == NULL || target == NULL)
        return -1;
    int32_t result = 0;
    k_linked_list_node_t* node = ke_linked_list_first(list);
    while(node != NULL)
    {
        if(equals != NULL)
        {
            if (equals(node, target))
            {
                return result;
            }
        }
        else
        {
            if(target->next == node->next && target->prev == node->prev)
            {
                return result;
            }
        }
        result++;
        node = ke_linked_list_next(node);
    }

    return -1;
}















