#include "lib/avl_tree.h"

static inline int32 SXAPI lbp_avl_tree_node_get_height(struct avl_tree_node *node)
{
	return node == NULL ? -1 : node->height;
}

static inline int32 SXAPI lbp_avl_tree_node_get_balance_factor(struct avl_tree_node *node)
{
	if (node == NULL)
	{
		return 0;
	}
	return lbp_avl_tree_node_get_height(node->left) - lbp_avl_tree_node_get_height(node->right);
}

static struct avl_tree_node *SXAPI lbp_avl_tree_node_right_rotate(struct avl_tree_node *root)
{
	struct avl_tree_node *left_children = root->left;
	//adjust parents first
	left_children->parent = root->parent;
	root->parent = left_children;
	if (left_children->right != NULL)
	{
		left_children->right->parent = root;
	}
	//perform rotation
	root->left = root->left->right;
	left_children->right = root;
	//adjust height
	root->height = lb_max_32(lbp_avl_tree_node_get_height(root->left), lbp_avl_tree_node_get_height(root->right)) + 1;
	left_children->height =
			lb_max_32(lbp_avl_tree_node_get_height(left_children->left),
			          lbp_avl_tree_node_get_height(left_children->right)) + 1;
	return left_children;
}

static struct avl_tree_node *SXAPI lbp_avl_tree_node_left_rotate(struct avl_tree_node *root)
{
	struct avl_tree_node *right_children = root->right;
	//adjust parents
	right_children->parent = root->parent;
	root->parent = right_children;
	if (right_children->left != NULL)
	{
		right_children->left->parent = root;
	}
	//perform rotation
	root->right = root->right->left;
	right_children->left = root;

	root->height = lb_max_32(lbp_avl_tree_node_get_height(root->left), lbp_avl_tree_node_get_height(root->right)) + 1;
	right_children->height =
			lb_max_32(lbp_avl_tree_node_get_height(right_children->left),
			          lbp_avl_tree_node_get_height(right_children->right)) +
			1;
	return right_children;
}

static struct avl_tree_node *SXAPI lbp_avl_tree_node_balance(struct avl_tree_node *node)
{
	const int32 bf = lbp_avl_tree_node_get_balance_factor(node);

	if (bf > 1)
	{
		const int32 left_bf = lbp_avl_tree_node_get_balance_factor(node->left);
		if (left_bf >= 0)
		{
			//left left
			return lbp_avl_tree_node_right_rotate(node);
		}
		else
		{
			//left right
			node->left = lbp_avl_tree_node_left_rotate(node->left);
			return lbp_avl_tree_node_right_rotate(node);
		}
	}
	else
	{
		if (bf < -1)
		{
			const int32 right_bf = lbp_avl_tree_node_get_balance_factor(node->right);
			if (right_bf <= 0)
			{
				// right right
				return lbp_avl_tree_node_left_rotate(node);
			}
			else
			{
				// right left
				node->right = lbp_avl_tree_node_right_rotate(node->right);
				return lbp_avl_tree_node_left_rotate(node);
			}
		}
		else
		{
			return node;
		}
	}

}

static struct avl_tree_node *SXAPI lbp_avl_tree_node_insert(struct avl_tree_node *root, struct avl_tree_node *node,
                                                            avl_tree_compare_func compare,
                                                            struct avl_tree_node *parent)
{
	if (node == NULL || compare == NULL)
	{
		return root;
	}
	if (root == NULL)
	{
		node->parent = parent;
		return node;
	}

	const int32 comp = compare(root, node);
	if (comp < 0)
	{
		root->right = lbp_avl_tree_node_insert(root->right, node, compare, root);
	}
	else
	{
		if (comp == 0)
		{
			return root;
		}
		else
		{
			root->left = lbp_avl_tree_node_insert(root->left, node, compare, root);
		}
	}

	root->height = lb_max_32(lbp_avl_tree_node_get_height(root->left), lbp_avl_tree_node_get_height(root->right)) + 1;

	return lbp_avl_tree_node_balance(root);
}

static void SXAPI lbp_avl_tree_swap_nodes(struct avl_tree_node *node1, struct avl_tree_node *node2)
{
	if (node1 == NULL || node2 == NULL)
	{
		return;
	}
	struct avl_tree_node *parent = NULL;
	struct avl_tree_node *child = NULL;
	struct avl_tree_node *temp = NULL;
	//swap node but does not change anything else other than node1,node2
	// determine the parent/child relationship
	if (node1->parent != NULL && node1->parent == node2)
	{
		parent = node2;
		child = node1;
	}
	else
	{
		if (node2->parent != NULL && node2->parent == node1)
		{
			parent = node1;
			child = node2;
		}
	}

	if (parent != NULL && child != NULL)
	{
		//connected case
		if (parent->parent != NULL)
		{
			if (parent->parent->left == parent)
			{
				parent->parent->left = child;
			}
			else
			{
				parent->parent->right = child;
			}
		}
		//update left/right for parent
		if (parent->left != NULL && child != parent->left)
		{
			parent->left->parent = child;
		}
		if (parent->right != NULL && child != parent->right)
		{
			parent->right->parent = child;
		}
		//update left/right for children
		if (child->left != NULL)
		{
			child->left->parent = parent;
		}
		if (child->right != NULL)
		{
			child->right->parent = parent;
		}

		child->parent = parent->parent;
		parent->parent = child;
		if (child == parent->left)
		{
			/*  parent
				 / \
			  children */
			parent->left = child->left;
			child->left = parent;

			temp = parent->right;
			parent->right = child->right;
			child->right = temp;
		}
		else
		{
			/*  parent
				 / \
				   children */
			parent->right = child->right;
			child->right = parent;

			temp = parent->left;
			parent->left = child->left;
			child->left = temp;
		}
	}
	else
	{
		//not connected case
		//adjust all the nodes other than node1,node2
		if (node1->left != NULL)
		{
			node1->left->parent = node2;
		}
		if (node1->right != NULL)
		{
			node1->right->parent = node2;
		}

		if (node2->left != NULL)
		{
			node2->left->parent = node1;
		}
		if (node2->right != NULL)
		{
			node2->right->parent = node1;
		}

		if (node1->parent != NULL)
		{
			if (node1->parent->left == node1)
			{
				node1->parent->left = node2;
			}
			else
			{
				node1->parent->right = node2;
			}
		}

		if (node2->parent != NULL)
		{
			if (node2->parent->left == node2)
			{
				node2->parent->left = node1;
			}
			else
			{
				node2->parent->right = node1;
			}
		}

		//adjust node1,node2
		temp = node1->parent;
		node1->parent = node2->parent;
		node2->parent = temp;

		temp = node1->left;
		node1->left = node2->left;
		node2->left = temp;

		temp = node1->right;
		node1->right = node2->right;
		node2->right = temp;
	}

	//swap height
	int32 height = node1->height;
	node1->height = node2->height;
	node2->height = height;
}

static struct avl_tree_node *SXAPI lbp_avl_tree_node_delete(struct avl_tree_node *root,
                                                            struct avl_tree_node *node,
                                                            avl_tree_compare_func compare,
                                                            struct avl_tree_node **deleted_node)
{
	if (root == NULL || node == NULL || compare == NULL || deleted_node == NULL)
	{
		return root;
	}
	const int32 comp = compare(root, node);
	if (comp < 0)
	{
		root->right = lbp_avl_tree_node_delete(root->right, node, compare, deleted_node);
	}
	else
	{
		if (comp > 0)
		{
			root->left = lbp_avl_tree_node_delete(root->left, node, compare, deleted_node);
		}
		else
		{
			*deleted_node = root;
			// node with only one child or no child
			if ((root->left == NULL) || (root->right == NULL))
			{
				struct avl_tree_node *child = root->left != NULL ? root->left : root->right;

				if (child == NULL)
				{   // 0 child
					root = NULL;
				}
				else // 1 child
				{
					child->parent = root->parent;
					root = child;
				}
			}
			else
			{
				// node with two children: Get the inorder successor (smallest
				// in the right subtree)
				struct avl_tree_node *successor = lb_avl_tree_larger(root);
				//swap fields
				lbp_avl_tree_swap_nodes(successor, root);

				// Detach the inorder successor
				successor->right = lbp_avl_tree_node_delete(successor->right, root, compare, deleted_node);

				root = successor;
			}
		}
	}
	if (root == NULL)
	{
		return root;
	}
	root->height = lb_max_32(lbp_avl_tree_node_get_height(root->left), lbp_avl_tree_node_get_height(root->right)) + 1;
	root = lbp_avl_tree_node_balance(root);
	return root;
}

static struct avl_tree_node *SXAPI lbp_avl_tree_node_search(struct avl_tree_node *root, struct avl_tree_node *node,
                                                            avl_tree_compare_func compare)
{
	if (root == NULL || compare == NULL)
	{
		return NULL;
	}
	const int32 comp = compare(root, node);
	if (comp < 0)
	{
		return lbp_avl_tree_node_search(root->right, node, compare);
	}
	else
	{
		if (comp == 0)
		{
			return root;
		}
		else
		{
			return lbp_avl_tree_node_search(root->left, node, compare);
		}
	}
}


static void SXAPI lbp_avl_tree_node_init(struct avl_tree_node *it)
{
	if (it != NULL)
	{
		it->height = 0;
		it->left = NULL;
		it->right = NULL;
		it->parent = NULL;
	}
}


struct avl_tree_node *SXAPI lb_avl_tree_smallest(struct avl_tree *tree)
{
	if (tree == NULL)
	{
		return NULL;
	}
	struct avl_tree_node *entry = tree->root;
	if (entry == NULL)
	{
		return NULL;
	}
	while (entry->left != NULL)
	{
		entry = entry->left;
	}
	return entry;
}

struct avl_tree_node *SXAPI lb_avl_tree_largest(struct avl_tree *tree)
{
	if (tree == NULL)
	{
		return NULL;
	}
	struct avl_tree_node *entry = tree->root;
	if (entry == NULL)
	{
		return NULL;
	}
	while (entry->right != NULL)
	{
		entry = entry->right;
	}
	return entry;
}


struct avl_tree_node *SXAPI lb_avl_tree_larger(struct avl_tree_node *it)
{
	if (it == NULL)
	{
		return NULL;
	}
	struct avl_tree_node *root = it;
	if (root->right != NULL)
	{
		root = root->right;
		while (root->left != NULL)
		{
			root = root->left;
		}
		return root;
	}
	else
	{
		while (root->parent != NULL)
		{
			if (root->parent->left == root)
			{
				return root->parent;
			}
			root = root->parent;
		}
		return NULL;
	}
}

struct avl_tree_node *SXAPI lb_avl_tree_smaller(struct avl_tree_node *it)
{
	if (it == NULL)
	{
		return NULL;
	}
	struct avl_tree_node *root = it;
	if (root->left != NULL)
	{
		root = root->left;
		while (root->right != NULL)
		{
			root = root->right;
		}
		return root;
	}
	else
	{
		while (root->parent != NULL)
		{
			if (root->parent->right == root)
			{
				return root->parent;
			}
			root = root->parent;
		}
		return NULL;
	}
}

struct avl_tree_node *SXAPI lb_avl_tree_search(struct avl_tree *tree, struct avl_tree_node *node)
{
	return lbp_avl_tree_node_search(tree->root, node, tree->compare);
}


void SXAPI lb_avl_tree_insert(struct avl_tree *tree, struct avl_tree_node *data)
{
	if (tree != NULL && data != NULL)
	{
		lbp_avl_tree_node_init(data);
		tree->root = lbp_avl_tree_node_insert(tree->root, data, tree->compare, NULL);
	}
}

struct avl_tree_node *SXAPI lb_avl_tree_delete(struct avl_tree *tree, struct avl_tree_node *data)
{
	struct avl_tree_node *node = NULL;
	if (tree != NULL && data != NULL)
	{
		tree->root = lbp_avl_tree_node_delete(tree->root, data, tree->compare, &node);
	}
	return node;
}

int32 SXAPI lb_avl_tree_size(struct avl_tree *tree)
{
	if (tree == NULL)
	{
		return -1;
	}
	if (tree->root == NULL)
	{
		return 0;
	}
	int32 size = 0;
	struct avl_tree_node *entry = lb_avl_tree_smallest(tree);
	while (entry != NULL)
	{
		size++;
		entry = lb_avl_tree_larger(entry);
	}
	return size;
}

void SXAPI lb_avl_tree_init(struct avl_tree *tree, avl_tree_compare_func compare)
{
	if (tree != NULL)
	{
		tree->compare = compare;
		tree->root = NULL;
	}
}



// TESTING STUFF

static int32 SXAPI lbp_avl_tree_node_calculate_height(struct avl_tree_node *tree)
{
	if (tree == NULL)
	{
		return -1;
	}
	return lb_max_32(lbp_avl_tree_node_calculate_height(tree->left), lbp_avl_tree_node_calculate_height(tree->right)) +
	       1;
}

static bool SXAPI lbp_avl_tree_node_test(struct avl_tree_node *tree, avl_tree_compare_func compare)
{
	if (tree == NULL)
	{
		return TRUE;
	}
	if (lbp_avl_tree_node_get_balance_factor(tree) < -1 || lbp_avl_tree_node_get_balance_factor(tree) > 1 ||
	    lbp_avl_tree_node_calculate_height(tree) != tree->height)
	{
		return FALSE;
	}
	if (tree->left != NULL)
	{
		if (tree->left->parent != tree)
		{
			return FALSE;
		}
	}
	if (tree->right != NULL)
	{
		if (tree->right->parent != tree)
		{
			return FALSE;
		}
	}
	if (compare != NULL)
	{
		if ((tree->right != NULL && compare(tree, tree->right) > 0) ||
		    (tree->left != NULL && compare(tree, tree->left) < 0))
		{
			return FALSE;
		}
	}
	return lbp_avl_tree_node_test(tree->left, compare) && lbp_avl_tree_node_test(tree->right, compare);
}

bool SXAPI lb_avl_tree_validate(struct avl_tree *tree)
{
	if (tree == NULL)
	{
		return TRUE;
	}
	return lbp_avl_tree_node_test(tree->root, tree->compare);
}
