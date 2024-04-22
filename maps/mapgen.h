#ifndef MAPGEN_H
#define MAPGEN_H

#define ROW 29
#define COL 27

#include "../pacman_ai/lib/linked_list.h"

typedef struct Dict {
    struct Index* head;
} Dict;

struct Index {
    struct Index* next;
    char* key;
    llist* value;
};

Dict* learn_mapping(char* path);

char* generate_map(Dict* dictionary);

#endif
