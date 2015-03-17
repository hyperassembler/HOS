#include "mm.h"
#include "linked_list.h"

//internal

linked_list_node* NATIVE64 _linked_list_get_by_index(linked_list_node *head, int index)
{
    while(1)
    {
        if(index <= 0 || head == NULL)
            return head;
        head = head->next;
        index--;
    }
}

linked_list_node* NATIVE64 _linked_list_get_by_element(linked_list_node *head, void *data, int(*compare)(int *, int *))
{
    while(1)
    {
        if(head == NULL || compare(data,head->data) == 0)
            return head;
        head = head->next;
    }
}


linked_list_node* NATIVE64 _linked_list_create_node()
{
    linked_list_node* node = (linked_list_node*)kmalloc(sizeof(linked_list_node));
    node->data = NULL;
    node->prev = NULL;
    node->next = NULL;
    return node;
}

void NATIVE64 _linked_list_delete_node(linked_list *list, linked_list_node *node)
{
    if(list != NULL && node != NULL)
    {
        //check if itself is head
        if(list->head == node)
        {
            list->head = node->next;
        }
        //check if itself is tail
        if(list->tail == node)
        {
            list->tail = node->prev;
        }
        if (node->prev != NULL)
            node->prev->next = node->next;
        if (node->next != NULL)
            node->next->prev = node->prev;
        list->size--;
        kfree(node);
    }
    return;
}

//interface
linked_list* NATIVE64 linked_list_create()
{
    linked_list* list = (linked_list*)kmalloc(sizeof(linked_list));
    list->size = 0;
    list->tail = NULL;
    list->head = NULL;
    return list;
}

void NATIVE64 linked_list_free(linked_list* list, void(*delete_data)(void*))
{
    if(list == NULL)
        return;
    linked_list_node* head = list->head;
    while(head != NULL)
    {
        linked_list_node* temp = head;
        head = head->next;
        if(delete_data != NULL)
            delete_data(temp->data);
        kfree(temp);
    }
    kfree(list);
    return;
}

void NATIVE64 linked_list_insert(linked_list * list, void* data)
{
    if(list == NULL)
        return;
    linked_list_node* node = _linked_list_create_node();
    node->data = data;
    if(list->tail != NULL)
    {
        //already elements in the list
        //guaranteed that list->next == NULL
        node->prev = list->tail;
        list->tail->next = node;
        list->tail = node;
    }
    else
    {
        //no element case
        list->tail = node;
        list->head = node;
    }
    list->size++;
    return;
}

void NATIVE64 linked_list_insert_at(linked_list * list, int position, void* data)
{
    if(list != NULL && position >= 0 && position <= list->size)
        {
        linked_list_node* target = _linked_list_get_by_index(list->head, position);
        if(target == NULL)
        {
            //tail case
            linked_list_insert(list, data);
        }
        else
        {
            //head or normal case
            linked_list_node* node = _linked_list_create_node();
            node->data = data;
            if (list->head == target) {
                list->head = node;
            }
            node->prev = target->prev;
            node->next = target;
            if (target->prev != NULL)
                target->prev->next = node;
            target->prev = node;
            list->size++;
        }
    }
    return;
}

void* NATIVE64 linked_list_get(linked_list * list, int index)
{
    if(list == NULL || list->head == NULL || index < 0 || list->size <= index)
        return NULL;
    linked_list_node* node = _linked_list_get_by_index(list->head, index);
    return node == NULL ? NULL : node->data;
}

void NATIVE64 linked_list_delete(linked_list * list, void* data, int(*compare)(int*,int*))
{
    if(list == NULL || list->head == NULL || compare == NULL)
        return;
    linked_list_node* node = _linked_list_get_by_element(list->head, data, compare);
    _linked_list_delete_node(list, node);
    return;
}

void NATIVE64 linked_list_delete_at(linked_list * list, int index)
{
    if(list == NULL || list->head == NULL || index < 0 || list->size <= index)
        return;
    linked_list_node* node = _linked_list_get_by_index(list->head, index);
    _linked_list_delete_node(list, node);
    return;
}

// iterator
linked_list_iterator* NATIVE64 linked_list_create_iterator(linked_list* list)
{
    if(list == NULL)
        return NULL;
    linked_list_iterator* it = (linked_list_iterator*)kmalloc(sizeof(linked_list_iterator));
    it->current = list->head;
    return it;
}

void NATIVE64 linked_list_delete_iterator(linked_list_iterator* it)
{
    kfree(it);
    return;
}

void NATIVE64 linked_list_prev(linked_list_iterator* it)
{
    it->current = it->current->prev;
    return;
}

void NATIVE64 linked_list_next(linked_list_iterator* it)
{
    it->current = it->current->next;
    return;
}