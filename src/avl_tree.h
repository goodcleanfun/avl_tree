#ifndef AVL_TREE_H
#define AVL_TREE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#endif // AVL_TREE_H

#ifndef AVL_TREE_NAME
#error "Must define AVL_TREE_NAME"
#endif

#ifndef AVL_TREE_KEY_TYPE
#error "Must define AVL_TREE_TYPE"
#endif

#ifndef AVL_TREE_VALUE_TYPE
#error "Must define AVL_TREE_VALUE_TYPE"
#endif

#ifndef AVL_TREE_MAX_HEIGHT
#define AVL_TREE_MAX_HEIGHT 128
#endif


#define AVL_CONCAT_(a, b) a ## b
#define AVL_CONCAT(a, b) AVL_CONCAT_(a, b)
#define AVL_TREE_TYPED(name) AVL_CONCAT(AVL_TREE_NAME, _##name)
#define AVL_TREE_FUNC(func) AVL_CONCAT(AVL_TREE_NAME, _##func)


#define BST_NAME AVL_TREE_NAME
#define BST_KEY_TYPE AVL_TREE_KEY_TYPE
#define BST_VALUE_TYPE AVL_TREE_VALUE_TYPE
#define BST_NODE_EXTRA \
    uint8_t height;

#ifdef AVL_TREE_KEY_EQUALS
#define BST_KEY_EQUALS AVL_TREE_KEY_EQUALS
#endif

#ifdef AVL_TREE_KEY_LESS_THAN
#define BST_KEY_LESS_THAN AVL_TREE_KEY_LESS_THAN
#endif

#include "binary_tree/binary_tree.h"

#undef BST_NAME
#undef BST_KEY_TYPE
#undef BST_VALUE_TYPE
#undef BST_NODE_EXTRA
#ifndef AVL_TREE_KEY_EQUALS
#define AVL_TREE_KEY_EQUALS AVL_TREE_TYPED(key_equals)
#endif
#ifndef AVL_TREE_KEY_LESS_THAN
#define AVL_TREE_KEY_LESS_THAN AVL_TREE_TYPED(key_less_than)
#endif

#define AVL_TREE_NODE AVL_TREE_TYPED(node_t)

#define AVL_TREE_NODE_MEMORY_POOL_NAME AVL_TREE_TYPED(node_memory_pool)

#define MEMORY_POOL_NAME AVL_TREE_NODE_MEMORY_POOL_NAME
#define MEMORY_POOL_TYPE AVL_TREE_NODE
#include "memory_pool/memory_pool.h"
#undef MEMORY_POOL_NAME
#undef MEMORY_POOL_TYPE

#define AVL_NODE_MEMORY_POOL_FUNC(name) AVL_CONCAT(AVL_TREE_NODE_MEMORY_POOL_NAME, _##name)


typedef struct AVL_TREE_NAME {
    AVL_TREE_TYPED(node_t) *root;
    AVL_TREE_TYPED(node_memory_pool) *pool;
} AVL_TREE_NAME;

AVL_TREE_NAME *AVL_TREE_FUNC(new)(void) {
    AVL_TREE_NAME *tree = malloc(sizeof(AVL_TREE_NAME));
    if (tree == NULL) return NULL;

    tree->pool = AVL_NODE_MEMORY_POOL_FUNC(new)();
    if (tree->pool == NULL) {
        free(tree);
        return NULL;
    }
    tree->root = AVL_NODE_MEMORY_POOL_FUNC(get)(tree->pool);
    if (tree->root == NULL) {
        AVL_NODE_MEMORY_POOL_FUNC(destroy)(tree->pool);
        free(tree);
        return NULL;
    }
    tree->root->left = NULL;
    tree->root->right = NULL;
    tree->root->height = 0;
    return tree;
}

void AVL_TREE_FUNC(destroy)(AVL_TREE_NAME *tree) {
    if (tree == NULL) return;
    AVL_NODE_MEMORY_POOL_FUNC(destroy)(tree->pool);
    free(tree);
}

bool AVL_TREE_FUNC(rebalance)(AVL_TREE_TYPED(stack_t) *stack) {
    AVL_TREE_NODE *tmp_node;
    bool done = false;
    // rebalance
    while (!AVL_TREE_FUNC(stack_empty)(stack) && !done) {
        uint8_t tmp_height, old_height;
        tmp_node = AVL_TREE_FUNC(stack_pop)(stack);
        if (AVL_TREE_FUNC(node_is_leaf)(tmp_node)) {
            // skip if there's a leaf node
            continue;
        }
        old_height = tmp_node->height;
        // left subtree is heavier
        if (tmp_node->left->height - tmp_node->right->height == 2) {
            if (tmp_node->left->left->height - tmp_node->right->height == 1) {
                // left subtree's left subtree is heavier, rotate right
                AVL_TREE_FUNC(rotate_right)(tmp_node);
                tmp_node->right->height = tmp_node->right->left->height + 1;
                tmp_node->height = tmp_node->right->height + 1;
            } else {
                // left subtree's right subtree is heavier, rotate left then right
                AVL_TREE_FUNC(rotate_left)(tmp_node->left);
                AVL_TREE_FUNC(rotate_right)(tmp_node);
                tmp_height = tmp_node->left->left->height;
                tmp_node->left->height = tmp_height + 1;
                tmp_node->right->height = tmp_height + 1;
                tmp_node->height = tmp_height + 2;
            }
        // right subtree is heavier
        } else if (tmp_node->right->height - tmp_node->left->height == 2) {
            if (tmp_node->right->right->height - tmp_node->left->height == 1) {
                // right subtree's right subtree is heavier, rotate left
                AVL_TREE_FUNC(rotate_left)(tmp_node);
                tmp_node->left->height = tmp_node->left->right->height + 1;
                tmp_node->height = tmp_node->left->height + 1;
            } else {
                // right subtree's left subtree is heavier, rotate right then left
                AVL_TREE_FUNC(rotate_right)(tmp_node->right);
                AVL_TREE_FUNC(rotate_left)(tmp_node);
                tmp_height = tmp_node->right->right->height;
                tmp_node->left->height = tmp_height + 1;
                tmp_node->right->height = tmp_height + 1;
                tmp_node->height = tmp_height + 2;
            }
        } else {
            // update height, no rotation needed
            if (tmp_node->left->height > tmp_node->right->height) {
                tmp_node->height = tmp_node->left->height + 1;
            } else {
                tmp_node->height = tmp_node->right->height + 1;
            }
        }
        if (tmp_node->height == old_height) {
            // terminate early if nothing changed heights
            done = true;
        }
    }
    return true;
}



bool AVL_TREE_FUNC(insert)(AVL_TREE_NAME *tree, AVL_TREE_KEY_TYPE key, void *value) {
    if (tree == NULL) return false;
    AVL_TREE_NODE *node = tree->root;
    AVL_TREE_TYPED(node_memory_pool) *pool = tree->pool;

    AVL_TREE_NODE *tmp_node;
    bool done = false;
    if (node->left == NULL) {
        // empty tree
        node->left = (AVL_TREE_NODE *)value;
        node->key = key;
        node->height = 0;
        node->right = NULL;
    } else {
        // non-empty tree
        AVL_TREE_TYPED(stack_t) stack = (AVL_TREE_TYPED(stack_t)){
            .stack = {NULL},
            .stack_size = 0
        };
        tmp_node = AVL_TREE_TYPED(candidate_leaf)(node, key, &stack);
        if (AVL_TREE_KEY_EQUALS(key, tmp_node->key)) {
            // key already exists
            return false;
        }
        // tmp_node is the leaf that will become the parent of the new leaf
        AVL_TREE_NODE *old_leaf = AVL_NODE_MEMORY_POOL_FUNC(get)(pool);
        if (old_leaf == NULL) return false;
        old_leaf->key = tmp_node->key;
        old_leaf->left = tmp_node->left;
        old_leaf->right = NULL;
        old_leaf->height = 0;
        AVL_TREE_NODE *new_leaf = AVL_NODE_MEMORY_POOL_FUNC(get)(pool);
        if (new_leaf == NULL) {
            AVL_NODE_MEMORY_POOL_FUNC(release)(pool, old_leaf);
            return false;
        }
        new_leaf->key = key;
        new_leaf->left = (AVL_TREE_NODE *)value;
        new_leaf->right = NULL;
        new_leaf->height = 0;
        if (AVL_TREE_KEY_LESS_THAN(tmp_node->key, key)) {
            tmp_node->left = old_leaf;
            tmp_node->right = new_leaf;
            tmp_node->key = key;
        } else {
            tmp_node->left = new_leaf;
            tmp_node->right = old_leaf;
        }
        tmp_node->height = 1;
        return AVL_TREE_FUNC(rebalance)(&stack);
    }
    return true;
}

void *AVL_TREE_FUNC(delete)(AVL_TREE_NAME *tree, AVL_TREE_KEY_TYPE key) {
    if (tree == NULL) return NULL;
    AVL_TREE_NODE *node = tree->root;
    AVL_TREE_TYPED(node_memory_pool) *pool = tree->pool;

    AVL_TREE_NODE *tmp_node, *upper_node, *other_node;
    void *deleted_value;
    if (node->left == NULL) {
        return NULL;
    } else if (node->right == NULL) {
        if (AVL_TREE_KEY_EQUALS(key, node->key)) {
            deleted_value = (void *)node->left;
            node->left = NULL;
            return deleted_value;
        } else {
            return NULL;
        }
    } else {
        tmp_node = node;
        AVL_TREE_TYPED(stack_t) stack = (AVL_TREE_TYPED(stack_t)){
            .stack = {NULL},
            .stack_size = 0
        };
        while (AVL_TREE_FUNC(node_is_internal)(tmp_node)) {
            if (!AVL_TREE_FUNC(stack_push(&stack, tmp_node))) return NULL;
            upper_node = tmp_node;
            if (AVL_TREE_KEY_LESS_THAN(key, tmp_node->key)) {
                tmp_node = upper_node->left;
                other_node = upper_node->right;
            } else {
                tmp_node = upper_node->right;
                other_node = upper_node->left;
            }
        }
        if (!AVL_TREE_KEY_EQUALS(key, tmp_node->key)) {
            return NULL;
        } else {
            upper_node->key = other_node->key;
            upper_node->left = other_node->left;
            upper_node->right = other_node->right;
            upper_node->height = other_node->height;
            deleted_value = (void *)tmp_node->left;
            // remove upper_node from the stack since it's now a leaf node
            AVL_TREE_FUNC(stack_pop)(&stack);

            AVL_NODE_MEMORY_POOL_FUNC(release)(pool, tmp_node);
            AVL_NODE_MEMORY_POOL_FUNC(release)(pool, other_node);
            if (!AVL_TREE_FUNC(rebalance)(&stack)) {
                return NULL;
            }
            return deleted_value;
        }
    }

}



#undef AVL_CONCAT_
#undef AVL_CONCAT
#undef AVL_TREE_TYPED
#undef AVL_TREE_FUNC
