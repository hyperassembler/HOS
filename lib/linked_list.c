#include "lib/linked_list.h"

static void SXAPI lbp_init_linked_list_node(linked_list_node_t *node)
{
	if (node != NULL)
	{
		node->next = NULL;
		node->prev = NULL;
	}
	return;
}

static void SXAPI lbp_append_node(linked_list_node_t *target, linked_list_node_t *node)
{
	if (target == NULL || node == NULL)
	{
		return;
	}

	linked_list_node_t *next = target->next;
	// update the next node
	if (next != NULL)
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
static void SXAPI lbp_prepend_node(linked_list_node_t *target, linked_list_node_t *node)
{
	if (target == NULL || node == NULL)
	{
		return;
	}

	linked_list_node_t *prev = target->prev;
	// update the prev node
	if (prev != NULL)
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

static void SXAPI lbp_unlink_node(linked_list_node_t *node)
{
	if (node == NULL)
	{
		return;
	}

	if (node->prev != NULL)
	{
		node->prev->next = node->next;
	}

	if (node->next != NULL)
	{
		node->next->prev = node->prev;
	}

	return;
}

void SXAPI lb_linked_list_init(linked_list_t *list)
{
	if (list != NULL)
	{
		list->head = NULL;
		list->tail = NULL;
	}
	return;
}

int32_t SXAPI lb_linked_list_size(linked_list_t *list)
{
	if (list == NULL)
	{
		return -1;
	}
	if (list->head == NULL)
	{
		return 0;
	}

	int32_t size = 1;
	linked_list_node_t *cur_node = list->head;
	linked_list_node_t *tail = list->tail;
	while ((cur_node != tail) && ((cur_node = cur_node->next) != NULL))
	{
		size++;
	}
	return size;
}

void SXAPI lb_linked_list_push_front(linked_list_t *list, linked_list_node_t *node)
{
	if (list == NULL || node == NULL)
	{
		return;
	}

	lbp_init_linked_list_node(node);

	lb_linked_list_insert_ref(list, NULL, node);

	return;
}

void SXAPI lb_linked_list_push_back(linked_list_t *list, linked_list_node_t *node)
{
	if (list == NULL || node == NULL)
	{
		return;
	}

	lbp_init_linked_list_node(node);

	lb_linked_list_insert_ref(list, list->tail, node);

	return;
}

linked_list_node_t *SXAPI lb_linked_list_pop_front(linked_list_t *list)
{
	if (list == NULL)
	{
		return NULL;
	}
	return lb_linked_list_remove_ref(list, list->head);
}

linked_list_node_t *SXAPI lb_linked_list_pop_back(linked_list_t *list)
{
	if (list == NULL)
	{
		return NULL;
	}

	return lb_linked_list_remove_ref(list, list->tail);
}


void SXAPI lb_linked_list_insert_ref(linked_list_t *list, linked_list_node_t *prev_node, linked_list_node_t *node)
{
	if (list == NULL || node == NULL)
	{
		return;
	}
	lbp_init_linked_list_node(node);
	if (prev_node == NULL)
	{
		// if prev_node is NULL, then we are inserting to the head

		// linked node with list->head
		lbp_prepend_node(list->head, node);

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
		lbp_append_node(prev_node, node);

		if (node->next == NULL)
		{
			// if it's the end
			list->tail = node;
		}
	}
}

void SXAPI lb_linked_list_insert(linked_list_t *list, int32_t index, linked_list_node_t *node)
{
	if (list == NULL || index < 0 || node == NULL)
	{
		return;
	}
	linked_list_node_t *prev_node = lb_linked_list_get(list, index - 1);
	lbp_init_linked_list_node(node);

	if (prev_node == NULL)
	{
		if (index == 0)
		{
			lb_linked_list_insert_ref(list, NULL, node);
		}
	}
	else
	{
		lb_linked_list_insert_ref(list, prev_node, node);
	}

	return;
}

linked_list_node_t *SXAPI lb_linked_list_remove(linked_list_t *list, int32_t index)
{
	if (list == NULL || index < 0)
	{
		return NULL;
	}
	linked_list_node_t *cur_node = lb_linked_list_get(list, index);

	if (cur_node == NULL)
	{
		return NULL;
	}

	return lb_linked_list_remove_ref(list, cur_node);
}

linked_list_node_t *SXAPI lb_linked_list_remove_ref(linked_list_t *list, linked_list_node_t *node)
{
	if (list == NULL || node == NULL)
	{
		return NULL;
	}

	lbp_unlink_node(node);

	if (node->next == NULL)
	{
		list->tail = node->prev;
	}

	if (node->prev == NULL)
	{
		list->head = node->next;
	}

	lbp_init_linked_list_node(node);

	return node;
}

linked_list_node_t *SXAPI lb_linked_list_get(linked_list_t *list, int32_t index)
{
	if (list == NULL || index < 0 || list->head == NULL)
	{
		return NULL;
	}
	linked_list_node_t *cur_node = list->head;
	while (index-- && (cur_node = cur_node->next) != NULL)
	{}
	return cur_node;
}

linked_list_node_t *SXAPI lb_linked_list_next(linked_list_node_t *node)
{
	if (node != NULL)
	{
		node = node->next;
	}
	return node;
}

linked_list_node_t *SXAPI lb_linked_list_prev(linked_list_node_t *node)
{
	if (node != NULL)
	{
		node = node->prev;
	}
	return node;
}

linked_list_node_t *SXAPI lb_linked_list_first(linked_list_t *list)
{
	linked_list_node_t *result = NULL;
	if (list != NULL)
	{
		result = list->head;
	}
	return result;
}

linked_list_node_t *SXAPI lb_linked_list_last(linked_list_t *list)
{
	linked_list_node_t *result = NULL;
	if (list != NULL)
	{
		result = list->tail;
	}
	return result;
}

int32_t SXAPI lb_linked_list_search(linked_list_t *list, linked_list_node_t *target,
                                   callback_func_t equals)
{
	if (list == NULL || target == NULL)
	{
		return -1;
	}
	int32_t result = 0;
	linked_list_node_t *node = lb_linked_list_first(list);
	while (node != NULL)
	{
		if (equals != NULL)
		{
			if (equals(node, target))
			{
				return result;
			}
		}
		else
		{
			if (target->next == node->next && target->prev == node->prev)
			{
				return result;
			}
		}
		result++;
		node = lb_linked_list_next(node);
	}

	return -1;
}















