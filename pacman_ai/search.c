#include <stddef.h>
#include <stdlib.h>
#include "lib/linked_list.h"
#include "utils.h"
#include "search.h"
#include "../src/map.h"
#include <limits.h>
#include "minimax.h"
#include <stdio.h>
#include <math.h>

#define WALL '#'
#define WALL2 '_'
#define GRID_HEIGHT 29
#define GRID_WIDTH 27
#define NOT_VISITED -5
#define VISITED 1

int *form_adjlist(){
    int* adj = calloc(4, sizeof(int));
    adj[0] = UP;
    adj[1] = LEFT;
    adj[2] = DOWN;
    adj[3] = RIGHT;
    return adj;
}

int convert(Ghost* g){
    switch (g->direction) {
        case up:
            return UP;
        case down:
            return DOWN;
        case left:
            return LEFT;
        case right:
            return RIGHT;
    }
    return 0;
}

int find_cost(Game* game, int pos){
    for (size_t i = 0; i < game->ghosts->length; i++){
        Ghost* g = llist_use(game->ghosts, i);
        int n = convert(g);
        int p = g->x + g->y * COL;       
        for (int i = 0; i < 3; i++){
            if (p == pos){
                return 10000;
            }
            p += n;
        }
    }
    if (game->map->grid[pos] == ' '){
        return 10;
    }
    if (game->map->grid[pos] == '#' || game->map->grid[pos] == '_'){
        return 100000;
    }
    return 1;
}

int Bellman(Game *game, size_t src){
    char* map = game->map->grid;
    int dist[COL * ROW];
    int p[COL * ROW];
    for (int i = 0; i < COL * ROW; i++){
        dist[i] = INT_MAX;
        p[i] = -1;
    }
    int n = COL * ROW;
    p[src] = 0;
    dist[src] = 0;
    while (n > 0){
        for (int x = 0; x < ROW * COL; x++){
            if (dist[x] != INT_MAX && map[x] != '#' && map[x] != '_'){
                int adjlist[4];
                adjlist[0] = UP;
                adjlist[1] = DOWN;
                adjlist[2] = LEFT;
                adjlist[3] = RIGHT;
                for (int y = 0; y < 4; y++){
                    int k = x + adjlist[y];
                    if (k < 0 || k >= ROW * COL){
                        continue;
                    }
                    if (map[k] == '#' || map[k] == '_'){
                        continue;
                    }
                    int cost = find_cost(game, k);
                    if (dist[x] + cost < dist[k]){
                        dist[k] = dist[x] + cost;
                        if (p[x] == 0){
                            p[k]= adjlist[y];
                        }
                        else{
                            p[k] = p[x];
                        }
                    }
                }
            }
        }
        n--;
    }
    int smallest = INT_MAX;
    int d = 0;
    for (size_t i = 0; i < COL * ROW; i++){
        if (map[i] != ' ' && map[i] != '#' && map[i] != '_'){
            if (dist[i] < smallest){
                d = p[i];
                smallest = dist[i];
            }
        }
    }
    return d;
}

int manhattandist(int x, int y, int i, int j){
    int a = abs(i - x);
    int b = abs(j - y);
    return a + b;
}

int min(int* dst, int len){
    int min_i = 0;
    for(int i = min_i; i < len; i++){
        if (dst[min_i] > dst[i]){
            min_i = i;
        }
    }
    return min_i;
}

int abs(int a){
    if (a < 0){
        a = -a;
    }
    return a;
}

int choose_dir(int dir, int* can_go){
    int lowest = INT_MAX;
    int lowest_i = 0;
    int dir_x = dir % COL;
    int dir_y = dir / COL;
    for (int i = 0; i < 4; i++){
        if (can_go[i] != 0){
            int can_x = can_go[i] % COL;
            int can_y = can_go[i] / COL;
            int man = manhattandist(dir_x, dir_y, can_x, can_y);
            if ( man < lowest){
                lowest = man;
                lowest_i = i;
            }
        }
    }
    printf("x:%d, y:%d\n", lowest % COL, lowest / COL);
    return can_go[lowest_i];
}

int closest(Game *game){
    int dst[ROW * COL];
    for (int j = 0; j < ROW; j++){
        for (int i = 0; i < COL; i++){
            if (game->map->grid[i + COL * j] == '.' || game->map->grid[i + COL * j] == 'o'){
                int x = game->pacman->x;
                int y = game->pacman->y;
                dst[i] = manhattandist(x, y, i, j);
            } 
            else{

                dst[i] = INT_MAX;
            }
        }
    }
    int lowest = min(dst, ROW * COL);
    int *can_go = can_goto(game);
    int c = choose_dir(lowest, can_go);
    free(can_go);
    return c;
}



int searchdumb(Game* game){
    int *options = can_goto(game);
    int last_i = 0;
    int not_dir = -game->pacman->direction;
    int pos = game->pacman->x + game->pacman->y * COL;
    for (int i = 0; i < 4; i++){
        if (game->map->grid[pos + options[i]] == '.' || game->map->grid[pos + options[i]] == 'o'){
            last_i = options[i];
            free(options);
            return last_i;
        }
        else{
            if (game->map->grid[pos + options[i]] == ' ' && options[i] != 0){
                if (options[i] != not_dir){
                    last_i = i;
                }
            }
        }
    }
    last_i = options[last_i];
    free(options);
    return last_i;
}
