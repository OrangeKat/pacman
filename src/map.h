#ifndef MAP_H
#define MAP_H

#include "../obj/player.h"
#include "../pacman_ai/lib/linked_list.h"

typedef struct Map{
    char* grid;
    int points;
    int num;
}Map;

int randomint(int min, int max);

Map *init_map();

void free_map(Map* map);

Map* load_map(char* path);

void save_map(Map* map, char* name);

void print_map(Map* map, Player* pacman, llist* ghosts);

#endif
