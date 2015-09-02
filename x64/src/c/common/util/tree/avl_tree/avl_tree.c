#include "../../../sys/kdef.h"
#include "avl_tree.h"

#define MAX(a, b) (((a) > (b) ? (a) : (b)))

int NATIVE64 _avl_tree_node_get_height(avl_tree_node_t *node)
{
    return node == NULL ? -1 : node->height;
}

int NATIVE64 _avl_tree_node_get_balance_factor(avl_tree_node_t *node)
{
    if (node == NULL)
        return 0;
    return _avl_tree_node_get_height(node->left) - _avl_tree_node_get_height(node->right);
}

avl_tree_node_t * NATIVE64 _avl_tree_node_right_rotate(avl_tree_node_t *root)
{
    avl_tree_node_t *left_children = root->left;
    //adjust parents first
    left_children->parent = root->parent;
    root->parent = left_children;
    if (left_children->right != NULL)
        left_children->right->parent = root;
    //perform rotation
    root->left = root->left->right;
    left_children->right = root;
    //adjust height
    root->height = MAX(_avl_tree_node_get_height(root->left), _avl_tree_node_get_height(root->right)) + 1;
    left_children->height = MAX(_avl_tree_node_get_height(left_children->left), _avl_tree_node_get_height(left_children->right)) + 1;
    return left_children;
}

avl_tree_node_t * NATIVE64 _avl_tree_node_left_rotate(avl_tree_node_t *root)
{
    avl_tree_node_t *right_children = root->right;
    //adjust parents
    right_children->parent = root->parent;
    root->parent = right_children;
    if (right_children->left != NULL)
        right_children->left->parent = root;
    //perform rotation
    root->right = root->right->left;
    right_children->left = root;

    root->height = MAX(_avl_tree_node_get_height(root->left), _avl_tree_node_get_height(root->right)) + 1;
    right_children->height = MAX(_avl_tree_node_get_height(right_children->left), _avl_tree_node_get_height(right_children->right)) + 1;
    return right_children;
}

avl_tree_node_t * NATIVE64 _avl_tree_node_balance(avl_tree_node_t *node)
{
    const int bf = _avl_tree_node_get_balance_factor(node);

    if (bf > 1)
    {
        const int left_bf = _avl_tree_node_get_balance_factor(node->left);
        if (left_bf >= 0)
            //left left
            return _avl_tree_node_right_rotate(node);
        else
        {
            //left right
            node->left = _avl_tree_node_left_rotate(node->left);
            return _avl_tree_node_right_rotate(node);
        }
    }
    else if (bf < -1)
    {
        const int right_bf = _avl_tree_node_get_balance_factor(node->right);
        if (right_bf <= 0)
        {
            // right right
            return _avl_tree_node_left_rotate(node);
        }
        else
        {
            // right left
            node->right = _avl_tree_node_right_rotate(node->right);
            return _avl_tree_node_left_rotate(node);
        }
    }
    else
        return node;

}

avl_tree_node_t * NATIVE64 _avl_tree_node_insert(avl_tree_node_t *root, avl_tree_node_t *node, int(*compare)(void *, void *), avl_tree_node_t *parent)
{
    if (node == NULL)
        return root;
    if (root == NULL)
    {
        node->parent = parent;
        return node;
    }

    const int comp = compare(root, node);
    if (comp < 0)
        root->right = _avl_tree_node_insert(root->right, node, compare, root);
    else if (comp == 0)
        return root;
    else
        root->left = _avl_tree_node_insert(root->left, node, compare, root);

    root->height = MAX(_avl_tree_node_get_height(root->left), _avl_tree_node_get_height(root->right)) + 1;

    return _avl_tree_node_balance(root);
}

void _swap_nodes(avl_tree_node_t *node1, avl_tree_node_t *node2)
{
    if (node1 == NULL || node2 == NULL)
        return;
    avl_tree_node_t *parent = NULL;
    avl_tree_node_t *child = NULL;
    avl_tree_node_t *temp = NULL;
    //swap node but does not change anything else other than node1,node2
    if (node1->parent != NULL && node1->parent == node2)
    {
        parent = node2;
        child = node1;
    }
    else if (node2->parent != NULL && node2->parent == node1)
    {
        parent = node1;
        child = node2;
    }

    if (parent != NULL && child != NULL)
    {
        //connected case
        if(parent->parent != NULL)
        {
            if(parent->parent->left == parent)
                parent->parent->left = child;
            else
                parent->parent->right = child;
        }
        //update left/right for parent
        if(parent->left != NULL && child != parent->left)
        {
            parent->left->parent = child;
        }
        if(parent->right != NULL && child != parent->right)
        {
            parent->right->parent = child;
        }
        //update left/right for children
        if(child->left != NULL)
            child->left->parent = parent;
        if(child->right != NULL)
            child->right->parent = parent;

        child->parent = parent->parent;
        parent->parent = child;
        if (child == parent->left)
        {
            //    parent
            //     / \
            //  children
            parent->left = child->left;
            child->left = parent;

            temp = parent->right;
            parent->right = child->right;
            child->right = temp;
        }
        else
        {
            //    parent
            //     / \
            //       children
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
        if(node1->left != NULL)
            node1->left->parent = node2;
        if(node1->right != NULL)
            node1->right->parent = node2;

        if(node2->left != NULL)
            node2->left->parent = node1;
        if(node2->right != NULL)
            node2->right->parent = node1;

        if(node1->parent != NULL)
        {
            if(node1->parent->left == node1)
                node1->parent->left = node2;
            else
                node1->parent->right = node2;
        }

        if(node2->parent != NULL)
        {
            if(node2->parent->left == node2)
                node2->parent->left = node1;
            else
                node2->parent->right = node1;
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
    int height = node1->height;
    node1->height = node2->height;
    node2->height = height;
    return;
}

//Interface
avl_tree_node_t * NATIVE64 avl_tree_node_insert(avl_tree_node_t * root, avl_tree_node_t * node, int(*compare)(void*,void*))
{
    return _avl_tree_node_insert(root, node, compare, NULL);
}

avl_tree_node_t * NATIVE64 avl_tree_node_delete(avl_tree_node_t * root, avl_tree_node_t * node, int (*compare)(void *, void *))
{
    if (root == NULL || node == NULL)
        return root;
    const int comp = compare(root, node);
    if (comp < 0)
        root->right = avl_tree_node_delete(root->right, node, compare);
    else if(comp > 0)
        root->left = avl_tree_node_delete(root->left, node, compare);
    else
    {
        // node with only one child or no child
        if( (root->left == NULL) || (root->right == NULL) )
        {
            avl_tree_node_t *child = root->left != NULL ? root->left : root->right;

            if(child == NULL)
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
            avl_tree_node_t *successor = root->right;
            while(successor->left != NULL)
                successor = successor->left;
            //swap fields
            _swap_nodes(successor,root);

            // Detach the inorder successor
            successor->right = avl_tree_node_delete(successor->right, root, compare);

            root = successor;
        }
    }
    if (root == NULL)
        return root;
    root->height = MAX(_avl_tree_node_get_height(root->left), _avl_tree_node_get_height(root->right)) + 1;
    root = _avl_tree_node_balance(root);
    return root;
}

avl_tree_node_t * NATIVE64 avl_tree_node_search(avl_tree_node_t *root, avl_tree_node_t * node, int(*compare)(void *, void *))
{
    if(root == NULL)
        return NULL;
    const int comp = compare(root, node);
    if (comp < 0)
        return avl_tree_node_search(root->right, node, compare);
    else if (comp == 0)
        return root;
    else
        return avl_tree_node_search(root->left, node, compare);
}

void NATIVE64 avl_tree_node_init(avl_tree_node_t * it)
{
    if(it != NULL)
    {
        it->height = 0;
        it->left = NULL;
        it->right = NULL;
        it->parent = NULL;
    }
    return;
}


avl_tree_node_t * NATIVE64 avl_tree_node_smallest(avl_tree_node_t *root)
{
    if (root == NULL)
        return NULL;
    while (root->left != NULL)
        root = root->left;
    return root;
}

avl_tree_node_t * NATIVE64 avl_tree_node_largest(avl_tree_node_t *root)
{
    if (root == NULL)
        return NULL;
    while (root->right != NULL)
        root = root->right;
    return root;
}


avl_tree_node_t * NATIVE64 avl_tree_node_next(avl_tree_node_t *it)
{
    if (it == NULL)
        return NULL;
    avl_tree_node_t * root = it;
    if (root->right != NULL)
    {
        root = root->right;
        while (root->left != NULL)
            root = root->left;
        return root;
    }
    else
    {
        while (root->parent != NULL)
        {
            if (root->parent->left == root)
                return root->parent;
            root = root->parent;
        }
        return NULL;
    }
}

avl_tree_node_t * NATIVE64 avl_tree_node_prev(avl_tree_node_t *it)
{
    if (it == NULL)
        return NULL;
    avl_tree_node_t * root = it;
    if (root->left != NULL)
    {
        root = root->left;
        while (root->right != NULL)
            root = root->right;
        return root;
    }
    else
    {
        while (root->parent != NULL)
        {
            if (root->parent->right == root)
                return root->parent;
            root = root->parent;
        }
        return NULL;
    }
}

avl_tree_node_t * avl_tree_search(avl_tree_t *tree, avl_tree_node_t * node, int (*compare)(void *, void *))
{
    return avl_tree_node_search(tree->root,node,compare);
}


void NATIVE64 avl_tree_insert(avl_tree_t *tree, void *data, int (*compare)(void *, void *))
{
    if(tree != NULL && data != NULL && compare != NULL)
    {
        if(avl_tree_search(tree, data, compare) == NULL)
        {
            tree->root = _avl_tree_node_insert(tree->root, data, compare, NULL);
            tree->size++;
        }
    }
    return;
}

void NATIVE64 avl_tree_delete(avl_tree_t *tree, void *data, int (*compare)(void *, void *))
{
    if(tree != NULL && data != NULL && compare != NULL && tree->size != 0)
    {
        if(avl_tree_search(tree, data, compare) != NULL)
        {
            tree->root = avl_tree_node_delete(tree->root, data, compare);
            tree->size--;
        }
    }
    return;
}

void NATIVE64 avl_tree_init(avl_tree_t * tree)
{
    if(tree != NULL)
    {
        tree->root = NULL;
        tree->size = 0;
    }
    return;
}
// TESTS

int NATIVE64 avl_tree_node_calculate_height(avl_tree_node_t *tree)
{
    if (tree == NULL)
        return -1;
    return MAX(avl_tree_node_calculate_height(tree->left), avl_tree_node_calculate_height(tree->right)) + 1;
}

int NATIVE64 avl_tree_node_test(avl_tree_node_t *tree, int (*compare)(void*,void*))
{
    if (tree == NULL)
        return 1;
    if (_avl_tree_node_get_balance_factor(tree) < -1 || _avl_tree_node_get_balance_factor(tree) > 1 || avl_tree_node_calculate_height(tree) != tree->height)
        return 0;
    if(tree->left != NULL)
    {
        if(tree->left->parent != tree)
            return 0;
    }
    if(tree->right != NULL)
    {
        if(tree->right->parent != tree)
            return 0;
    }
    if(compare != NULL)
    {
        if((tree->right != NULL && compare(tree,tree->right) > 0) || (tree->left != NULL && compare(tree,tree->left) < 0))
            return 0;
    }
    return avl_tree_node_test(tree->left,compare) && avl_tree_node_test(tree->right,compare);
}

