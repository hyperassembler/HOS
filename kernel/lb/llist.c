#include "lp.h"

static void
llist_node_init(struct llist_node *node)
{
    node->next = NULL;
    node->prev = NULL;
}


void
lb_llist_init(struct llist *list)
{
    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
}


uint32
lb_llist_size(struct llist *list)
{
    return list->size;
}


void
lb_llist_push_front(struct llist *list, struct llist_node *node)
{
    llist_node_init(node);
    lb_llist_insert_by_ref(list, NULL, node);
}


void
lb_llist_push_back(struct llist *list, struct llist_node *node)
{
    llist_node_init(node);
    lb_llist_insert_by_ref(list, list->tail, node);
}


struct llist_node *
lb_llist_pop_front(struct llist *list)
{
    struct llist_node *ret;

    ret = list->head;
    lb_llist_remove_by_ref(list, list->head);

    return ret;
}


struct llist_node *
lb_llist_pop_back(struct llist *list)
{
    struct llist_node *ret;

    ret = list->tail;
    lb_llist_remove_by_ref(list, list->tail);

    return ret;
}


void
lb_llist_insert_by_ref(struct llist *list, struct llist_node *cur_node, struct llist_node *new_node)
{
    struct llist_node *left_node;
    struct llist_node *right_node;

    if (list == NULL || new_node == NULL)
    {
        return;
    }

    llist_node_init(new_node);

    /*
     * adjust the current node
     */
    if (cur_node == NULL)
    {
        new_node->next = list->head;
        new_node->prev = NULL;
    }
    else
    {
        new_node->prev = cur_node;
        new_node->next = cur_node->next;
    }

    /*
     * assign left and treenode node
     */
    if (cur_node == NULL)
    {
        left_node = NULL;
        right_node = list->head == NULL ? NULL : list->head;
    }
    else
    {
        left_node = cur_node;
        right_node = cur_node->next;
    }

    /*
     * adjust left and treenode node accordingly
     */
    if (left_node != NULL)
    {
        left_node->next = new_node;
    }
    else
    {
        list->head = new_node;
    }

    if (right_node != NULL)
    {
        right_node->prev = new_node;
    }
    else
    {
        list->tail = new_node;
    }

    list->size++;
}


void
lb_llist_insert_by_idx(struct llist *list, uint32 index, struct llist_node *node)
{
    struct llist_node *prev_node;

    prev_node = lb_llist_get(list, index - 1);
    llist_node_init(node);

    if (prev_node == NULL)
    {
        if (index == 0)
        {
            lb_llist_insert_by_ref(list, NULL, node);
        }
    }
    else
    {
        lb_llist_insert_by_ref(list, prev_node, node);
    }
}


struct llist_node *
lb_llist_remove_by_idx(struct llist *list, uint32 index)
{
    struct llist_node *cur_node;

    cur_node = lb_llist_get(list, index);

    if (cur_node == NULL)
    {
        return NULL;
    }

    return lb_llist_remove_by_ref(list, cur_node);
}


/**
 * returns the next node
 */
struct llist_node *
lb_llist_remove_by_ref(struct llist *list, struct llist_node *node)
{
    struct llist_node *ret;

    /*
     * Adjust the left and treenode node
     */
    if (node->prev != NULL)
    {
        node->prev->next = node->next;
    }
    else
    {
        list->head = node->next;
    }

    if (node->next != NULL)
    {
        node->next->prev = node->prev;
    }
    else
    {
        list->tail = node->prev;
    }

    ret = node->next;

    llist_node_init(node);

    list->size--;

    return ret;
}


struct llist_node *
lb_llist_get(struct llist *list, uint32 index)
{
    if (list->head == NULL)
    {
        return NULL;
    }
    struct llist_node *cur_node = list->head;
    while (index-- && (cur_node = cur_node->next) != NULL)
    {}
    return cur_node;
}


struct llist_node *
lb_llist_next(struct llist_node *node)
{
    return node->next;
}


struct llist_node *
lb_llist_prev(struct llist_node *node)
{
    return node->prev;
}


struct llist_node *
lb_llist_first(struct llist *list)
{
    return list->head;
}


struct llist_node *
lb_llist_last(struct llist *list)
{
    return list->tail;
}

