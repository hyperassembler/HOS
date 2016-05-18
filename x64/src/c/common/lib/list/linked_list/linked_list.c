#include "linked_list.h"

static void SAPI _init_linked_list_node(linked_list_node_t *node)
{
    if (node != NULL)
    {
        node->next = NULL;
        node->prev = NULL;
    }
    return;
}

void SAPI linked_list_init(linked_list_t *list)
{
    if (list != NULL)
    {
        list->head = NULL;
        list->tail = NULL;
    }
    return;
}

int SAPI linked_list_size(linked_list_t *list)
{
    if (list == NULL)
        return -1;
    if (list->head == NULL)
        return 0;

    int size = 1;
    linked_list_node_t *cur_node = list->head;
    linked_list_node_t *tail = list->tail;
    while ((cur_node != tail) && ((cur_node = cur_node->next) != NULL))
    {
        size++;
    }
    return size;
}

void SAPI linked_list_push_front(linked_list_t *list, linked_list_node_t *node)
{
    if (list == NULL || node == NULL)
        return;

    _init_linked_list_node(node);

    linked_list_insert_ref(list, NULL, node);

    return;
}

void SAPI linked_list_push_back(linked_list_t *list, linked_list_node_t *node)
{
    if (list == NULL || node == NULL)
        return;

    _init_linked_list_node(node);

    linked_list_insert_ref(list, list->tail, node);

    return;
}

linked_list_node_t *SAPI linked_list_pop_front(linked_list_t *list)
{
    if (list == NULL)
        return NULL;
    return linked_list_remove_ref(list, list->head);
}

linked_list_node_t *SAPI linked_list_pop_back(linked_list_t *list)
{
    if (list == NULL)
        return NULL;

    return linked_list_remove_ref(list, list->tail);
}


void SAPI linked_list_insert_ref(linked_list_t *list, linked_list_node_t *prev_node, linked_list_node_t *node)
{
    if (list == NULL || node == NULL)
        return;
    _init_linked_list_node(node);
    if (prev_node == NULL)
    {
        // if prev_node is NULL, then we are inserting to the head
        if (list->head == NULL)
        {
            // if the list is empty
            list->head = node;
            list->tail = node;
        }
        else
        {
            // not empty
            linked_list_node_t *cur_node = list->head;
            node->next = cur_node;
            node->prev = cur_node->prev;
            cur_node->prev = node;
            list->head = node;
        }
    }
    else
    {
        // if prev_node is not NULL, we are inserting to the middle or the end
        if (prev_node->next != NULL)
        {
            // if not the end
            linked_list_node_t *next_node = prev_node->next;

            // add to the chain
            next_node->prev = node;
            prev_node->next = node;
            node->prev = prev_node;
            node->next = next_node;
        }
        else
        {
            // we are inserting at the end of the list
            prev_node->next = node;
            node->prev = prev_node;
            list->tail = node;
        }
    }
}

void SAPI linked_list_insert_idx(linked_list_t *list, int index, linked_list_node_t *node)
{
    if (list == NULL || index < 0 || node == NULL)
        return;
    linked_list_node_t *prev_node = linked_list_get(list, index - 1);
    _init_linked_list_node(node);

    if (prev_node == NULL)
    {
        if (index == 0)
        {
            linked_list_insert_ref(list, NULL, node);
        }
    }
    else
    {
        linked_list_insert_ref(list, prev_node, node);
    }

    return;
}

linked_list_node_t *SAPI linked_list_remove_idx(linked_list_t *list, int index)
{
    if (list == NULL || index < 0)
        return NULL;
    linked_list_node_t *cur_node = linked_list_get(list, index);

    if (cur_node == NULL)
        return NULL;

    return linked_list_remove_ref(list, cur_node);
}

linked_list_node_t *SAPI linked_list_remove_ref(linked_list_t *list, linked_list_node_t *node)
{
    if (list == NULL || node == NULL)
        return NULL;

    if (node->next == NULL && node->prev == NULL)
    {
        // the only node
        list->head = NULL;
        list->tail = NULL;
    }

    if (node->next != NULL)
    {
        // there is something after node
        linked_list_node_t *next_node = node->next;
        next_node->prev = node->prev;

        if (node->prev == NULL)
        {
            // first element
            list->head = next_node;
        }
    }

    if (node->prev != NULL)
    {
        // there is something before the node
        linked_list_node_t *prev_node = node->prev;
        prev_node->next = node->next;
        if (node->next == NULL)
        {
            // last element
            list->tail = prev_node;
        }
    }

    _init_linked_list_node(node);

    return node;
}

linked_list_node_t *SAPI linked_list_get(linked_list_t *list, int index)
{
    if (list == NULL || index < 0 || list->head == NULL)
        return NULL;
    linked_list_node_t *cur_node = list->head;
    while (index-- && (cur_node = cur_node->next) != NULL);
    return cur_node;
}

linked_list_node_t *SAPI linked_list_next(linked_list_node_t *node)
{
    if (node != NULL)
    {
        node = node->next;
    }
    return node;
}

linked_list_node_t *SAPI linked_list_prev(linked_list_node_t *node)
{
    if (node != NULL)
    {
        node = node->prev;
    }
    return node;
}

linked_list_node_t *SAPI linked_list_first(linked_list_t *list)
{
    linked_list_node_t *result = NULL;
    if (list != NULL)
    {
        result = list->head;
    }
    return result;
}

linked_list_node_t *SAPI linked_list_last(linked_list_t *list)
{
    linked_list_node_t *result = NULL;
    if (list != NULL)
    {
        result = list->tail;
    }
    return result;
}













