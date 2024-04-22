#ifndef GHOST_H
#define GHOST_H

// This file will handle all the ghost ai and structs

// Structure of the ghost AI.
// This file will handle all the ghost ai and structs
#include "player.h"
#include "../src/map.h"

#define ROW 29
#define COL 27
#define WALL '#'
#define WALL2 '_'

typedef enum GhostMode
{
    SCATTER,
    CHASE,
    FRIGHTENED,
    DEAD
}GhostMode;


typedef enum GhostDirection
{
	up,
	down,
	left,
	right
}GhostDirection;

typedef struct Ghost
{
    char* name;
    int x;
    int y;
    GhostDirection direction;
    GhostMode mode;
    int targetX;
    int targetY;
    int scatterTargetX;
    int scatterTargetY;
    int spawnX;
    int spawnY;
}Ghost;

// Ghost functions


// Initialize a ghost
Ghost* ghostInit(char* label, int direction, int mode, 
        int scatterTargetX, int scatterTargetY, int spawnX, int spawnY);

// check the number of adjacent walls
int checkWalls(char* map, int x, int y);

// Moves the ghost and checks for collisions + intersections with the map
void GhostMove(Ghost* ghost, Ghost* blinky, char* map, struct Player* player);

// Just moves the ghost (however no checks are made)
void move(Ghost* ghost);

// Checks if there is a wall in front of the ghost and changes direction
void checkWall(Ghost* ghost, char* map);

// When the ghost is at an intersection with 0 or 1 adjacent walls 
// recalculate the target of the ghost
// depending on the mode
// There is a need to input blinky for inky's target calulation in CHASE mode
void ghostPathing(Ghost* ghost, Ghost* blinky, char* map, 
        Player* player, int nb_walls);

#endif
