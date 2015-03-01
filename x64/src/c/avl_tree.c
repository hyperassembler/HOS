#include "avl_tree.h"
#include <stdlib.h>

#define MAX(a, b) (((a) > (b) ? (a) : (b)))

// internal
int inline _get_height(avl_tree *node)
{
    return node == NULL ? -1 : node->height;
}

int _balance_factor(avl_tree *node)
{
    if (node == NULL)
        return 0;
    return _get_height(node->left) - _get_height(node->right);
}

avl_tree *_right_rotate(avl_tree *root)
{
    avl_tree *left_children = root->left;
    //adjust parents first
    left_children->parent = root->parent;
    root->parent = left_children;
    if (left_children->right != NULL)
        left_children->right->parent = root;
    //perform rotation
    root->left = root->left->right;
    left_children->right = root;
    //adjust height
    root->height = MAX(_get_height(root->left), _get_height(root->right)) + 1;
    left_children->height = MAX(_get_height(left_children->left), _get_height(left_children->right)) + 1;
    return left_children;
}

avl_tree *_left_rotate(avl_tree *root)
{
    avl_tree *right_children = root->right;
    //adjust parents
    right_children->parent = root->parent;
    root->parent = right_children;
    if (right_children->left != NULL)
        right_children->left->parent = root;
    //perform rotation
    root->right = root->right->left;
    right_children->left = root;

    root->height = MAX(_get_height(root->left), _get_height(root->right)) + 1;
    right_children->height = MAX(_get_height(right_children->left), _get_height(right_children->right)) + 1;
    return right_children;
}

avl_tree *_balance_node(avl_tree *node)
{
    const int bf = _balance_factor(node);

    if (bf > 1)
    {
        const int left_bf = _balance_factor(node->left);
        if (left_bf >= 0)
            //left left
            return _right_rotate(node);
        else
        {
            //left right
            node->left = _left_rotate(node->left);
            return _right_rotate(node);
        }
    }
    else if (bf < -1)
    {
        const int right_bf = _balance_factor(node->right);
        if (right_bf <= 0)
        {
            // right right
            return _left_rotate(node);
        }
        else
        {
            // right left
            node->right = _right_rotate(node->right);
            return _left_rotate(node);
        }
    }
    else
        return node;

}

avl_tree *_create()
{
    avl_tree *tree = (avl_tree *) (malloc(sizeof(avl_tree)));
    tree->parent = NULL;
    tree->data = NULL;
    tree->right = NULL;
    tree->left = NULL;
    tree->height = 0;
    return tree;
}

avl_tree *_insert(avl_tree *root, void *data, int(*compare)(void *, void *), avl_tree *parent)
{
    if (data == NULL)
        return root;
    if (root == NULL)
    {
        avl_tree *tree = _create();
        tree->data = data;
        tree->parent = parent;
        return tree;
    }

    const int comp = compare(root->data, data);
    if (comp < 0)
    {
        root->right = _insert(root->right, data, compare, root);
    }
    else if (comp == 0)
        return root;
    else
        root->left = _insert(root->left, data, compare, root);

    root->height = MAX(_get_height(root->left), _get_height(root->right)) + 1;

    return _balance_node(root);
}

// implementation

void *avl_read(avl_tree *root)
{
    if (root == NULL)
        return NULL;
    return root->data;
}

avl_tree *avl_next(avl_tree *root)
{
    if (root == NULL)
        return NULL;
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

avl_tree *avl_prev(avl_tree *root)
{
    if (root == NULL)
        return NULL;
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

avl_tree *avl_insert(avl_tree *root, void *data, int (*compare)(void *, void *))
{
    return _insert(root, data, compare, NULL);
}

avl_tree *avl_search(avl_tree *root, void *data, int(*compare)(void *, void *))
{
    if(root == NULL)
        return NULL;
    const int comp = compare(root->data, data);
    if (comp < 0)
    {
         return avl_search(root->right, data, compare);
    }
    else if (comp == 0)
        return root;
    else
        return avl_search(root->left, data, compare);
}

avl_tree *avl_create()
{
    return NULL;
}

avl_tree *avl_smallest(avl_tree *root)
{
    if (root == NULL)
        return NULL;
    while (root->left != NULL)
        root = root->left;
    return root;
}

avl_tree *avl_largest(avl_tree *root)
{
    if (root == NULL)
        return NULL;
    while (root->right != NULL)
        root = root->right;
    return root;
}

int avl_size(avl_tree *avl_tree)
{
    if (avl_tree == NULL)
        return 0;
    return avl_size(avl_tree->left) + avl_size(avl_tree->right) + 1;
}

void avl_free(avl_tree *root, void (*delete_data)(void*))
{
    if (root == NULL)
        return;
    avl_free(root->left, delete_data);
    avl_free(root->right, delete_data);
    if(delete_data != NULL)
        delete_data(root->data);
    free(root);
    return;
}

avl_tree* avl_delete(avl_tree* root, void* data, int (*compare)(void*,void*))
{
    if (root == NULL)
        return NULL;
    const int comp = compare(root->data, data);
    if (comp < 0)
        root->right = avl_delete(root->right, data, compare);
    else if(comp > 0)
        root->left = avl_delete(root->left, data, compare);
    else
    {
        // node with only one child or no child
        if( (root->left == NULL) || (root->right == NULL) )
        {
            avl_tree *child = root->left != NULL ? root->left : root->right;

            if(child == NULL)
            {   // 0 child
                free(root);
                root = NULL;
            }
            else // 1 child
            {
                //copy content of temp to root except for the parent
                root->left = child->left;
                root->right = child->right;
                root->data = child->data;
                root->height = child->height;
                free(child);
            }
        }
        else
        {
            // node with two children: Get the inorder successor (smallest
            // in the right subtree)

            avl_tree* temp = root->right;
            while(temp->left != NULL)
                temp = temp->left;

            // Copy the inorder successor's data to this node
            root->data = temp->data;

            // Delete the inorder successor
            root->right = avl_delete(root->right, temp->data, compare);
        }
    }
    if (root == NULL)
        return root;
    root->height = MAX(_get_height(root->left), _get_height(root->right)) + 1;
    root = _balance_node(root);
    return root;
}


// TESTS

int avl_test_calculate_height(avl_tree *avl_tree)
{
    if (avl_tree == NULL)
        return -1;
    return MAX(avl_test_calculate_height(avl_tree->left), avl_test_calculate_height(avl_tree->right)) + 1;
}

int avl_test(avl_tree *avl_tree)
{
    if (avl_tree == NULL)
        return 1;
    if (_balance_factor(avl_tree) < -1 || _balance_factor(avl_tree) > 1 || avl_test_calculate_height(avl_tree) != avl_tree->height)
        return 0;
    return avl_test(avl_tree->left) && avl_test(avl_tree->right);
}

