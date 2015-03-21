#include "kdef.h"
#include "linked_list.h"

void NATIVE64 linked_list_node_init(linked_list_node* node)
{
    if(node != NULL)
    {
        node->prev = NULL;
        node->next = NULL;
    }
    return;
}

linked_list_node* NATIVE64 linked_list_node_get(linked_list_node * head, int index)
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

int NATIVE64 linked_list_node_size(linked_list_node* head)
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
linked_list_node* NATIVE64 linked_list_node_insert(linked_list_node * head, int index, linked_list_node* node)
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
        linked_list_node* target = linked_list_node_get(head, index-1);
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

void NATIVE64 linked_list_node_add(linked_list_node * head, linked_list_node* node)
{
    if(head == NULL || node == NULL)
        return;
    int size = linked_list_node_size(head);
    linked_list_node_insert(head,size,node);
    return;
}

//returns new head
linked_list_node* NATIVE64 linked_list_node_remove(linked_list_node *head, int index)
{
    if(head == NULL || index < 0)
        return head;
    if(index == 0)
    {
        linked_list_node* next = head->next;
        head->next = NULL;
        head->prev = NULL;
        if(next != NULL)
            next->prev = NULL;
        return next;
    }
    else
    {
        linked_list_node *target = linked_list_node_get(head, index);
        if (target->prev != NULL)
            target->prev->next = target->next;
        if (target->next != NULL)
            target->next->prev = target->prev;
        target->prev = NULL;
        target->next = NULL;
        return head;
    }
}


void NATIVE64 linked_list_init(linked_list* list)
{
    if(list != NULL)
    {
        list->size = 0;
        list->head = NULL;
    }
    return;
}

void NATIVE64 linked_list_add(linked_list * list, linked_list_node* node)
{
    if(list != NULL && node != NULL)
    {
        //@ node != NULL
        if (list->head == NULL)
        {
            list->head = node;
        }
        else
            linked_list_node_add(list->head, node);
        list->size++;
    }
    return;
}

void NATIVE64 linked_list_insert(linked_list * list, int index, linked_list_node* node)
{
    if(list != NULL && index > 0 && node != NULL)
    {
        list->head = linked_list_node_insert(list->head, index, node);
        list->size++;
    }
    return;
}

linked_list_node* NATIVE64 linked_list_get(linked_list * list, int index)
{
    if(list == NULL || index < 0 || index >= list->size)
        return NULL;
    return linked_list_node_get(list->head,index);
}

void NATIVE64 linked_list_remove(linked_list *list, int index)
{
    if(list != NULL && index >= 0 && index < list->size)
    {
        list->head = linked_list_node_remove(list->head, index);
        list->size--;
    }
    return;
}