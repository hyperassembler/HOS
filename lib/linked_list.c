#include "lib/linked_list.h"

static void SXAPI lbp_init_linked_list_node(struct linked_list_node *node)
{
	if (node != NULL)
	{
		node->next = NULL;
		node->prev = NULL;
	}
}

static void SXAPI lbp_append_node(struct linked_list_node *target, struct linked_list_node *node)
{
	if (target == NULL || node == NULL)
	{
		return;
	}

	struct linked_list_node *next = target->next;
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
}

// link target with node, suppose target is in the current list
static void SXAPI lbp_prepend_node(struct linked_list_node *target, struct linked_list_node *node)
{
	if (target == NULL || node == NULL)
	{
		return;
	}

	struct linked_list_node *prev = target->prev;
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
}

static void SXAPI lbp_unlink_node(struct linked_list_node *node)
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
}

void SXAPI lb_linked_list_init(struct linked_list *list)
{
	if (list != NULL)
	{
		list->head = NULL;
		list->tail = NULL;
	}
}

int32 SXAPI lb_linked_list_size(struct linked_list *list)
{
	if (list == NULL)
	{
		return -1;
	}
	if (list->head == NULL)
	{
		return 0;
	}

	int32 size = 1;
	struct linked_list_node *cur_node = list->head;
	struct linked_list_node *tail = list->tail;
	while ((cur_node != tail) && ((cur_node = cur_node->next) != NULL))
	{
		size++;
	}
	return size;
}

void SXAPI lb_linked_list_push_front(struct linked_list *list, struct linked_list_node *node)
{
	if (list == NULL || node == NULL)
	{
		return;
	}

	lbp_init_linked_list_node(node);

	lb_linked_list_insert_ref(list, NULL, node);
}

void SXAPI lb_linked_list_push_back(struct linked_list *list, struct linked_list_node *node)
{
	if (list == NULL || node == NULL)
	{
		return;
	}

	lbp_init_linked_list_node(node);

	lb_linked_list_insert_ref(list, list->tail, node);
}

struct linked_list_node *SXAPI lb_linked_list_pop_front(struct linked_list *list)
{
	if (list == NULL)
	{
		return NULL;
	}
	return lb_linked_list_remove_ref(list, list->head);
}

struct linked_list_node *SXAPI lb_linked_list_pop_back(struct linked_list *list)
{
	if (list == NULL)
	{
		return NULL;
	}

	return lb_linked_list_remove_ref(list, list->tail);
}


void SXAPI lb_linked_list_insert_ref(struct linked_list *list, struct linked_list_node *prev_node, struct linked_list_node *node)
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

void SXAPI lb_linked_list_insert(struct linked_list *list, int32 index, struct linked_list_node *node)
{
	if (list == NULL || index < 0 || node == NULL)
	{
		return;
	}
	struct linked_list_node *prev_node = lb_linked_list_get(list, index - 1);
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
}

struct linked_list_node *SXAPI lb_linked_list_remove(struct linked_list *list, int32 index)
{
	if (list == NULL || index < 0)
	{
		return NULL;
	}
	struct linked_list_node *cur_node = lb_linked_list_get(list, index);

	if (cur_node == NULL)
	{
		return NULL;
	}

	return lb_linked_list_remove_ref(list, cur_node);
}

struct linked_list_node *SXAPI lb_linked_list_remove_ref(struct linked_list *list, struct linked_list_node *node)
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

struct linked_list_node *SXAPI lb_linked_list_get(struct linked_list *list, int32 index)
{
	if (list == NULL || index < 0 || list->head == NULL)
	{
		return NULL;
	}
	struct linked_list_node *cur_node = list->head;
	while (index-- && (cur_node = cur_node->next) != NULL)
	{}
	return cur_node;
}

struct linked_list_node *SXAPI lb_linked_list_next(struct linked_list_node *node)
{
	if (node != NULL)
	{
		node = node->next;
	}
	return node;
}

struct linked_list_node *SXAPI lb_linked_list_prev(struct linked_list_node *node)
{
	if (node != NULL)
	{
		node = node->prev;
	}
	return node;
}

struct linked_list_node *SXAPI lb_linked_list_first(struct linked_list *list)
{
	struct linked_list_node *result = NULL;
	if (list != NULL)
	{
		result = list->head;
	}
	return result;
}

struct linked_list_node *SXAPI lb_linked_list_last(struct linked_list *list)
{
	struct linked_list_node *result = NULL;
	if (list != NULL)
	{
		result = list->tail;
	}
	return result;
}

int32 SXAPI lb_linked_list_search(struct linked_list *list, struct linked_list_node *target,
                                   callback_func equals)
{
	if (list == NULL || target == NULL)
	{
		return -1;
	}
	int32 result = 0;
	struct linked_list_node *node = lb_linked_list_first(list);
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

