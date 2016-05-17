#include "../../../sys/kdef.h"
#include "linked_list.h"

static void SAPI linked_list_node_init(linked_list_node_t * node)
{
    if(node != NULL)
    {
        node->prev = NULL;
        node->next = NULL;
    }
    return;
}

static linked_list_node_t *SAPI linked_list_node_get(linked_list_node_t * head, int index)
{
    if(head == NULL || index < 0)
        return NULL;
    while(index--)
    {
        head = head->next;
        if(head == NULL)
            break;
    }
    return head;
}

static int SAPI linked_list_node_size(linked_list_node_t * head)
{
    int i = 0;
    while(head != NULL)
    {
        i++;
        head = head->next;
    }
    return i;
}

//returns new head
static linked_list_node_t *SAPI linked_list_node_insert(linked_list_node_t * head, int index, linked_list_node_t * node)
{
    if(head == NULL)
        return node;
    if(node == NULL || index < 0)
        return head;
    //@ head != NULL
    if(index == 0)
    {
        //insert at head
        node->prev = NULL;
        node->next = head;
        head->prev = node;
        return node;
    }
    else
    {
        linked_list_node_t * target = linked_list_node_get(head, index-1);
        if(target == NULL)
            return NULL;
        node->prev = target;
        node->next = target->next;
        if(target->next != NULL)
            target->next->prev = node;
        target->next = node;
        return head;
    }
}

static void SAPI linked_list_node_push_back(linked_list_node_t *head, linked_list_node_t *node)
{
    if(head == NULL || node == NULL)
        return;
    int size = linked_list_node_size(head);
    linked_list_node_insert(head,size,node);
    return;
}

//returns new head
static linked_list_node_t *SAPI linked_list_node_remove(linked_list_node_t *head, int index)
{
    if(head == NULL || index < 0)
        return head;
    if(index == 0)
    {
        linked_list_node_t * next = head->next;
        linked_list_node_init(head);
        if(next != NULL)
            next->prev = NULL;
        return next;
    }
    else
    {
        linked_list_node_t *target = linked_list_node_get(head, index);
        if (target->prev != NULL)
            target->prev->next = target->next;
        if (target->next != NULL)
            target->next->prev = target->prev;
        linked_list_node_init(target);
        return head;
    }
}


void SAPI linked_list_init(linked_list_t * list)
{
    if(list != NULL)
    {
        list->size = 0;
        list->head = NULL;
    }
    return;
}

void SAPI linked_list_push_back(linked_list_t *list, linked_list_node_t *node)
{
    if(list != NULL && node != NULL)
    {
        //@ node != NULL
        linked_list_node_init(node);
        if (list->head == NULL)
        {
            list->head = node;
        }
        else
            linked_list_node_push_back(list->head, node);
        list->size++;
    }
    return;
}

void SAPI linked_list_insert(linked_list_t * list, int index, linked_list_node_t * node)
{
    if(list != NULL && index > 0 && node != NULL)
    {
        linked_list_node_init(node);
        list->head = linked_list_node_insert(list->head, index, node);
        list->size++;
    }
    return;
}

linked_list_node_t *SAPI linked_list_get(linked_list_t * list, int index)
{
    if(list == NULL || index < 0 || index >= list->size)
        return NULL;
    return linked_list_node_get(list->head,index);
}

void SAPI linked_list_remove(linked_list_t *list, int index)
{
    if(list != NULL && index >= 0 && index < list->size)
    {
        list->head = linked_list_node_remove(list->head, index);
        list->size--;
    }
    return;
}