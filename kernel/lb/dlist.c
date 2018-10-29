#include "lb/dlist.h"

static void
llist_node_init(struct dlist_node *node)
{
    node->next = NULL;
    node->prev = NULL;
}


void
lb_llist_init(struct dlist *list)
{
    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
}


uint32
lb_llist_size(struct dlist *list)
{
    return list->size;
}

void
lb_llist_insert(struct dlist *list, struct dlist_node *cur_node, struct dlist_node *new_node)
{
    struct dlist_node *left_node;
    struct dlist_node *right_node;

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

/**
 * returns the next node
 */
struct dlist_node *
lb_llist_remove(struct dlist *list, struct dlist_node *node)
{
    struct dlist_node *ret;

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


struct dlist_node *
lb_llist_next(struct dlist_node *node)
{
    return node->next;
}


struct dlist_node *
lb_llist_prev(struct dlist_node *node)
{
    return node->prev;
}


struct dlist_node *
lb_llist_first(struct dlist *list)
{
    return list->head;
}


struct dlist_node *
lb_llist_last(struct dlist *list)
{
    return list->tail;
}

