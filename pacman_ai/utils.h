#ifndef UTILS_H
#define UTILS_H

struct Astar_node{
    struct Astar_node *parent;
    int position;
    int g, h, f;
};

void init_astar_node(struct Astar_node *node, struct Astar_node *parent, int position);

void free_astar_node(struct Astar_node *node);

#endif

