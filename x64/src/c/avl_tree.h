#ifndef _AVL_TREE_H_
#define _AVL_TREE_H_
#include <stdint.h>
#include "kdef.h"

void*NATIVE64 insert_node(void *node, void *key,
        void *(*get_left)(void *),
        void (*set_left)(void *, void *),
        void *(*get_right)(void *),
        void(*set_right)(void *, void *),
        int64_t (*get_height)(void *),
        void(*set_height)(void *, int64_t),
        int (*compare)(void *, void *));

void*NATIVE64 delete_node(void *root, void *key,
        void *(*get_left)(void *),
        void (*set_left)(void *, void *),
        void *(*get_right)(void *),
        void(*set_right)(void *, void *),
        int64_t (*get_height)(void *),
        void(*set_height)(void *, int64_t),
        int (*compare)(void *, void *),
        void (*set_data)(void *, void *));
#endif