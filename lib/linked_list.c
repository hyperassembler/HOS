#include "lib/linked_list.h"

static inline void SXAPI lbp_init_linked_list_node(struct linked_list_node *node)
{
	if (node != NULL)
	{
		node->next = NULL;
		node->prev = NULL;
	}
}

void SXAPI lb_linked_list_init(struct linked_list *list)
{
	if (list != NULL)
	{
		list->head = NULL;
		list->tail = NULL;
		list->size = 0;
	}
}

int32 SXAPI lb_linked_list_size(struct linked_list *list)
{
	if (list == NULL)
	{
		return -1;
	}

	return list->size;
}

void SXAPI lb_linked_list_push_front(struct linked_list *list, struct linked_list_node *node)
{
	if (list == NULL || node == NULL)
	{
		return;
	}

	lbp_init_linked_list_node(node);

	lb_linked_list_insert_by_ref(list, NULL, node);
}

void SXAPI lb_linked_list_push_back(struct linked_list *list, struct linked_list_node *node)
{
	if (list == NULL || node == NULL)
	{
		return;
	}

	lbp_init_linked_list_node(node);

	lb_linked_list_insert_by_ref(list, list->tail, node);
}

struct linked_list_node *SXAPI lb_linked_list_pop_front(struct linked_list *list)
{
	if (list == NULL)
	{
		return NULL;
	}
	return lb_linked_list_remove_by_ref(list, list->head);
}

struct linked_list_node *SXAPI lb_linked_list_pop_back(struct linked_list *list)
{
	if (list == NULL)
	{
		return NULL;
	}

	return lb_linked_list_remove_by_ref(list, list->tail);
}


void SXAPI lb_linked_list_insert_by_ref(struct linked_list *list, struct linked_list_node *cur_node,
                                        struct linked_list_node *new_node)
{
	struct linked_list_node *left_node = NULL;
	struct linked_list_node *right_node = NULL;

	if (list == NULL || new_node == NULL)
	{
		return;
	}

	lbp_init_linked_list_node(new_node);

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
	 * assign left and right node
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
	 * adjust left and right node accordingly
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

void SXAPI lb_linked_list_insert_by_idx(struct linked_list *list, int32 index, struct linked_list_node *node)
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
			lb_linked_list_insert_by_ref(list, NULL, node);
		}
	}
	else
	{
		lb_linked_list_insert_by_ref(list, prev_node, node);
	}
}

struct linked_list_node *SXAPI lb_linked_list_remove_by_idx(struct linked_list *list, int32 index)
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

	return lb_linked_list_remove_by_ref(list, cur_node);
}

/*
 * returns the next node
 */
struct linked_list_node *SXAPI lb_linked_list_remove_by_ref(struct linked_list *list, struct linked_list_node *node)
{
	struct linked_list_node *ret = NULL;

	if (list == NULL || node == NULL)
	{
		return ret;
	}

	/*
	 * Adjust the left and right node
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

	lbp_init_linked_list_node(node);

	list->size--;

	return ret;
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

struct linked_list_node *SXAPI lb_linked_list_search(struct linked_list *list, void *obj, linked_list_cmp_func cmp_func)
{
	struct linked_list_node *ret = NULL;
	struct linked_list_node *node = lb_linked_list_first(list);

	if (list == NULL)
	{
		return NULL;
	}

	while (node != NULL)
	{
		if (cmp_func(node, obj) == 0)
		{
			ret = node;
			break;
		}

		node = lb_linked_list_next(node);
	}

	return ret;
}

