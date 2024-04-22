#include "utils.h"
#include <stddef.h>
#include <stdlib.h>

/*
Astar queue and node Functions
*/

void init_astar_node(struct Astar_node *node, struct Astar_node *parent, int position){
    node->f = 0;
    node->g = 0;
    node->h = 0;
    node->parent = parent;
    node->position = position;
}

void free_astar_node(struct Astar_node *node){
    free(node);
}
