#ifndef TREE_H
#define TREE_H

#include "linked_list.h"

typedef struct gtree {
        llist *children;
        void *key;
} gtree;

void init_gtree(gtree *tree);

void gtree_free(gtree *tree);

#endif
