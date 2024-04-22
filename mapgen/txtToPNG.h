#ifndef TXT_TO_PNG
#define TXT_TO_PNG

// Here we will take charge of transforming map in txt format to a png format
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "stdio.h"
#include "stddef.h"
#include <err.h>
// From the 10 default sprites build the map in PNG format with the map in 
// an array
void combinePNGs(const char* outputFilename, char* map);

// get the number of adjacent walls and builds and array of the adjacent walls
// returns a heap allocated char array of length 9 representing 3x3 grid of
// adjacent walls, the center being the current position
// 1 -> wall
// 2 -> houseWall
// 0 -> no wall
// -1 -> center
// house: 1 if there is houseWall, 0 otherwise
char* nbAdjacentWalls(char* grid, int x, int y, int* nbWalls, int* house);

// choose the tile to use depending on the number of adjacent walls and position
int chooseTile(char* tiles, int nbWalls, int house);

#endif
