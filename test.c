#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "greatest/greatest.h"

#define AVL_TREE_NAME avl_tree_uint32
#define AVL_TREE_KEY_TYPE uint32_t
#define AVL_TREE_VALUE_TYPE char *
#include "avl_tree.h"
#undef AVL_TREE_NAME
#undef AVL_TREE_KEY_TYPE
#undef AVL_TREE_VALUE_TYPE

TEST test_avl_tree(void) {
    avl_tree_uint32 *tree = avl_tree_uint32_new();

    char *val1 = "a";
    char *val2 = "b";
    char *val3 = "c";

    avl_tree_uint32_insert(tree, 1, "a");
    avl_tree_uint32_insert(tree, 3, "b");
    avl_tree_uint32_insert(tree, 5, "c");
    avl_tree_uint32_insert(tree, 7, "d");
    avl_tree_uint32_insert(tree, 9, "e");

    char *a = avl_tree_uint32_search(tree->root, 1);
    ASSERT_STR_EQ(a, "a");

    char *b = avl_tree_uint32_search(tree->root, 3);
    ASSERT_STR_EQ(b, "b");

    char *c = avl_tree_uint32_search(tree->root, 5);
    ASSERT_STR_EQ(c, "c");

    char *d = avl_tree_uint32_search(tree->root, 7);
    ASSERT_STR_EQ(d, "d");

    char *e = avl_tree_uint32_search(tree->root, 9);
    ASSERT_STR_EQ(e, "e");

    a = avl_tree_uint32_delete(tree, 1);
    ASSERT_STR_EQ(a, "a");
  
    a = avl_tree_uint32_search(tree->root, 1);
    ASSERT(a == NULL);
  
    b = avl_tree_uint32_delete(tree, 3);
    ASSERT_STR_EQ(b, "b");

    e = avl_tree_uint32_delete(tree, 9);
    ASSERT_STR_EQ(e, "e");

    c = avl_tree_uint32_search(tree->root, 5);
    ASSERT_STR_EQ(c, "c");

    c = avl_tree_uint32_delete(tree, 5);
    ASSERT_STR_EQ(c, "c");

    d = avl_tree_uint32_delete(tree, 7);
    ASSERT_STR_EQ(d, "d");

    d = avl_tree_uint32_search(tree->root, 7);
    ASSERT(d == NULL);

    avl_tree_uint32_insert(tree, 7, "d");
    d = avl_tree_uint32_search(tree->root, 7);

    avl_tree_uint32_destroy(tree);
    PASS();
}



/* Add definitions that need to be in the test runner's main file. */
GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
    GREATEST_MAIN_BEGIN();      /* command-line options, initialization. */

    RUN_TEST(test_avl_tree);

    GREATEST_MAIN_END();        /* display results */
}