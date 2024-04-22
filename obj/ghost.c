#include "ghost.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>

Ghost* ghostInit(char* label, int direction, int mode,
 int scatterTargetX, int scatterTargetY, int spawnX, int spawnY)
{
    Ghost* ghost = malloc(sizeof(Ghost));
    ghost->name = label;
    ghost->x = spawnX;
    ghost->y = spawnY;
    ghost->direction = direction;
    ghost->mode = mode;
    ghost->targetX = scatterTargetX;
    ghost->targetY = scatterTargetY;
    ghost->scatterTargetX = scatterTargetX;
    ghost->scatterTargetY = scatterTargetY;
    ghost->spawnX = spawnX;
    ghost->spawnY = spawnY;
    return ghost;
}

void move(Ghost* ghost){
    // depending on the direction of the ghost, move it
    switch (ghost->direction)
    {
        case up:
            ghost->y -= 1;
            break;
        case down:
            ghost->y += 1;
            break;
        case left:
            ghost->x -= 1;
            break;
        case right:
            ghost->x += 1;
            break;
    }
}

int checkWalls(char* map, int x, int y){
    int nb_walls = 0;
    if (map[(y - 1) * COL + x] == WALL ||
            map[(y - 1) * COL + x] == WALL2)
        nb_walls++;
    if (map[(y + 1) * COL + x] == WALL ||
            map[(y + 1) * COL + x] == WALL2)
        nb_walls++;
    if (map[y * COL + x - 1] == WALL ||
            map[y * COL + x - 1] == WALL2)
        nb_walls++;
    if (map[y * COL + x + 1] == WALL ||
            map[y * COL + x + 1] == WALL2)
        nb_walls++;
    return nb_walls;
}

void GhostMove(Ghost* ghost, Ghost* blinky, char* map, struct Player* player){
    int nb_walls = checkWalls(map, ghost->x, ghost->y);
    if (ghost->mode == DEAD){
        if (ghost->x == 13 && ghost->y == 11)
            ghost->y += 1;
        else if (ghost->x == 13 && ghost->y == 12)
            ghost->y += 1;
        else {
            ghostPathing(ghost, blinky, map, player, 0);
            move(ghost);
        }

        return;
    }
    // case where the ghost is still in the ghost house
    if (map[(ghost->y - 1) * COL + ghost->x] == WALL2){
        ghost->y -= 1;
        return;
    }
    if (map[ghost->y * COL + ghost->x] == WALL2){
        ghost->y -= 1;
        return;
    }
    //printf("nb_walls: %d\n", nb_walls);
    //printf("ghost name: %s, nb_walls: %d, x:%d, y:%d, targetx:%d, targety:%d\n", ghost->name,nb_walls, ghost->x, ghost->y, ghost->targetX, ghost->targetY);
    // if there are 0/1 walls around the ghost then we can use pathing
    // This is basically saying that we are at an intersection
    if (nb_walls <= 1)
        ghostPathing(ghost, blinky, map, player, nb_walls);
    else
        checkWall(ghost, map);
    move(ghost);

    //printf("ghost name: %s, direction: %d\n", ghost->name, ghost->direction);
}

// if direction is NULL then we only check the ghost direction
void checkWall(Ghost* ghost, char* map){
    //check the next position of the ghost and if it's a wall then change the direction to the only direction it can go
    switch (ghost->direction)
    {
        case up:
            if(map[((ghost->y - 1) * COL) + ghost->x]== WALL)
            {
                if(map[(ghost->y * COL) + ghost->x - 1] != WALL)
                {
                    ghost->direction = left;
                }
                else if(map[(ghost->y * COL) + ghost->x + 1] != WALL)
                {
                    ghost->direction = right;
                }
            }
            break;
        case down:
            if(map[((ghost->y + 1) * COL) + ghost->x] == WALL)
            {
                if(map[(ghost->y * COL) + ghost->x - 1] != WALL)
                {
                    ghost->direction = left;
                }
                else if(map[(ghost->y * COL) + ghost->x + 1] != WALL)
                {
                    ghost->direction = right;
                }
            }
            break;
        case left:
            if(map[(ghost->y * COL) + ghost->x - 1] == WALL)
            {
                if(map[((ghost->y - 1) * COL) + ghost->x] != WALL)
                {
                    ghost->direction = up;
                }
                else if(map[((ghost->y + 1) * COL) + ghost->x] != WALL)
                {
                    ghost->direction = down;
                }
            }
            break;
        case right:
            if(map[ghost->y * COL + ghost->x + 1] == WALL)
            {
                if(map[((ghost->y - 1) * COL) + ghost->x] != WALL)
                {
                    ghost->direction = up;
                }
                else if(map[((ghost->y + 1) * COL) + ghost->x] != WALL)
                {
                    ghost->direction = down;
                }
            }
            break;
    }
}

int manhattanDistance(int x1, int y1, int x2, int y2){
    return abs(x1 - x2) + abs(y1 - y2);
}

// when at an intersection the ghost will choose the direction 
// that will bring it closer to the target
GhostDirection updateDirection(int ghostx, int ghosty,
        int targetx, int targety, char* map, GhostDirection direction,
        GhostMode mode, int nb_walls){
    int min_distance = INT_MAX; // arbitrary large number
    GhostDirection new_direction = direction;
    switch(direction)
    {
        case up:
            if(map[((ghosty - 1) * COL) + ghostx] != WALL)
            {
                if (mode == DEAD || nb_walls >= 1){
                    if (manhattanDistance(ghostx, ghosty - 1, targetx, targety) < min_distance){
                        min_distance = manhattanDistance(ghostx, ghosty - 1, targetx, targety);
                        new_direction = up;
                    }
                }
                
            }
            if(map[(ghosty * COL) + ghostx - 1] != WALL)
            {
                if (manhattanDistance(ghostx - 1, ghosty, targetx, targety) < min_distance){
                    min_distance = manhattanDistance(ghostx - 1, ghosty, targetx, targety);
                    new_direction = left;
                }
            }
            if(map[(ghosty * COL) + ghostx + 1] != WALL)
            {
                if (manhattanDistance(ghostx + 1, ghosty, targetx, targety) < min_distance){
                    min_distance = manhattanDistance(ghostx + 1, ghosty, targetx, targety);
                    new_direction = right;
                }
            }
            break;
        case down:
            if(map[((ghosty + 1) * COL) + ghostx] != WALL)
            {
               if (manhattanDistance(ghostx, ghosty + 1, targetx, targety) < min_distance){
                    min_distance = manhattanDistance(ghostx, ghosty + 1, targetx, targety);
                    new_direction = down;
                }
            }
            if(map[(ghosty * COL) + ghostx - 1] != WALL)
            {
                if (manhattanDistance(ghostx - 1, ghosty, targetx, targety) < min_distance){
                    min_distance = manhattanDistance(ghostx - 1, ghosty, targetx, targety);
                    new_direction = left;
                }
            }
            if(map[(ghosty * COL) + ghostx + 1] != WALL)
            {
                if (manhattanDistance(ghostx + 1, ghosty, targetx, targety) < min_distance){
                    min_distance = manhattanDistance(ghostx + 1, ghosty, targetx, targety);
                    new_direction = right;
                }
            }
            break;
        case left:
            if(map[((ghosty - 1) * COL) + ghostx] != WALL)
            {
                if (manhattanDistance(ghostx, ghosty - 1, targetx, targety) < min_distance){
                    min_distance = manhattanDistance(ghostx, ghosty - 1, targetx, targety);
                    new_direction = up;
                }
            }
            if(map[((ghosty + 1) * COL) + ghostx] != WALL)
            {
                if (manhattanDistance(ghostx, ghosty + 1, targetx, targety) < min_distance){
                    min_distance = manhattanDistance(ghostx, ghosty + 1, targetx, targety);
                    new_direction = down;
                }
            }
            if(map[(ghosty * COL) + ghostx - 1] != WALL)
            {
                if (manhattanDistance(ghostx - 1, ghosty, targetx, targety) < min_distance){
                    min_distance = manhattanDistance(ghostx - 1, ghosty, targetx, targety);
                    new_direction = left;
                }
            }
            break;
        case right:
            if(map[((ghosty - 1) * COL) + ghostx] != WALL)
            {
                if (manhattanDistance(ghostx, ghosty - 1, targetx, targety) < min_distance){
                    min_distance = manhattanDistance(ghostx, ghosty - 1, targetx, targety);
                    new_direction = up;
                }
            }
            if(map[((ghosty + 1) * COL) + ghostx] != WALL)
            {
                if (manhattanDistance(ghostx, ghosty + 1, targetx, targety) < min_distance){
                    min_distance = manhattanDistance(ghostx, ghosty + 1, targetx, targety);
                    new_direction = down;
                }
            }
            if(map[(ghosty * COL) + ghostx + 1] != WALL)
            {
                if (manhattanDistance(ghostx + 1, ghosty, targetx, targety) < min_distance){
                    min_distance = manhattanDistance(ghostx + 1, ghosty, targetx, targety);
                    new_direction = right;
                }
            }
            break;
    }
    return new_direction;
}

void ghostPathing(Ghost* ghost, Ghost* blinky, char* map, 
        Player* player, int nb_walls)
{
    // Depending on the ghost mode, and knowing that the ghost is at an
    // intersection, then choose the direction of the ghost
    switch (ghost->mode)
    {
        case SCATTER:
            // Target the ghost's scatter target
            ghost->targetX = ghost->scatterTargetX;
            ghost->targetY = ghost->scatterTargetY;
            ghost->direction = updateDirection(ghost->x, ghost->y,
                    ghost->targetX, ghost->targetY, map, ghost->direction,
                    ghost->mode, nb_walls);
            break;
        case CHASE:
            // Target the ghost's chase target
            if (strcmp(ghost->name, "blinky") == 0)
            {
                ghost->targetX = player->x;
                ghost->targetY = player->y;
                ghost->direction = updateDirection(ghost->x, ghost->y,
                    ghost->targetX, ghost->targetY, map, ghost->direction,
                    ghost->mode, nb_walls);

            }
            else if (strcmp(ghost->name, "pinky") == 0)
            {
                //depending on the direction of the player
                //target 4 tiles ahead of the player
                switch (player->direction){
                    case up:
                        ghost->targetX = player->x;
                        ghost->targetY = player->y - 4;
                        break;
                    case down:
                        ghost->targetX = player->x;
                        ghost->targetY = player->y + 4;
                        break;
                    case left:
                        ghost->targetX = player->x - 4;
                        ghost->targetY = player->y;
                        break;
                    case right:
                        ghost->targetX = player->x + 4;
                        ghost->targetY = player->y;
                        break;
                }
                ghost->direction = updateDirection(ghost->x, ghost->y,
                    ghost->targetX, ghost->targetY, map, ghost->direction,
                    ghost->mode, nb_walls);
            }
            else if (strcmp(ghost->name, "inky") == 0)
            {
                int dpx = 0 ;
                int dpy = 0;
                switch (player->direction){
                    case up:
                        dpx = player->x;
                        dpy = player->y - 2;
                        break;
                    case down:
                        dpx = player->x;
                        dpy = player->y + 2;
                        break;
                    case left:
                        dpx = player->x - 2;
                        dpy = player->y;
                        break;
                    case right:
                        dpx = player->x + 2;
                        dpy = player->y;
                        break;
                }
                int blinkyx = blinky->x;
                int blinkyy = blinky->y;
                ghost->targetX = 2 * dpx - blinkyx;
                ghost->targetY = 2 * dpy - blinkyy;
                ghost->direction = updateDirection(ghost->x, ghost->y,
                    ghost->targetX, ghost->targetY, map, ghost->direction,
                    ghost->mode, nb_walls);
            }
            else if (strcmp(ghost->name, "clyde") == 0)
            {
                int dx = ghost->x - player->x;
                int dy = ghost->y - player->y;
                if (dx * dx + dy * dy > 64)
                {
                    ghost->targetX = player->x;
                    ghost->targetY = player->y;
                }
                else
                {
                    ghost->targetX = ghost->scatterTargetX;
                    ghost->targetY = ghost->scatterTargetY;
                }
                ghost->direction = updateDirection(ghost->x, ghost->y,
                    ghost->targetX, ghost->targetY, map, ghost->direction,
                    ghost->mode, nb_walls);
            }
            break;
        case FRIGHTENED:
            // Target a random position
            ghost->targetX = rand() % COL;
            ghost->targetY = rand() % ROW;
            ghost->direction = updateDirection(ghost->x, ghost->y,
                    ghost->targetX, ghost->targetY, map, ghost->direction,
                    ghost->mode, nb_walls);
            break;
        case DEAD:
            // Target the ghost's spawn position
            ghost->targetX = ghost->spawnX;
            ghost->targetY = ghost->spawnY;
            ghost->direction = updateDirection(ghost->x, ghost->y,
                    ghost->targetX, ghost->targetY, map, ghost->direction,
                    ghost->mode, nb_walls);
            break;
        
    }
}
