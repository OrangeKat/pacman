#ifndef SEARCH_H
#define SEARCH_H

#include "utils.h"
#include "../main.h"

// bfs traversal of the grid and adds the coords of the path to reach the end
// in the stack

int Bellman(Game* game, size_t src);

int bfs(Game *game);

int closest(Game *game);

int searchdumb(Game *game);

#endif
