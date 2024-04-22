#include "linked_list.h"
#include "tree.h"
#include <stdlib.h>

void init_gtree(gtree *tree){
    llist *children = init_llist();
    tree = malloc(sizeof(gtree));
    tree->children = children;
}

void gtree_free(gtree *tree){
    struct Node *child = tree->children->head;
    struct Node *tmp;
    while (child != NULL){
        gtree_free(child->data);
        tmp = child->next;
        free(child->data);
        free(child);
        child = tmp;
    }
    llist_free(tree->children);
    free(tree->key);
    free(tree);
}
