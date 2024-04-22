#include "txtToPNG.h"
#include <unistd.h>
#define SPRITE_SIZE 16
#define ROW 29
#define COL 27
#define WALL '#'
#define WALL2 '_'

char* nbAdjacentWalls(char* grid, int x, int y, int* nbWalls, int* house)
{
    // Check the 8 adjacent tiles and increment nbWalls for each wall found
    // -> check if the tile is a wall
    // -> increment nbWalls if it is
    // -> return the string of the 8 adjacent tiles
    char* adjacentTiles = calloc(9, sizeof(char));
    *nbWalls = 0;

    int i_start = (x == 0) ? 0 : -1;
    int j_start = (y == 0) ? 0 : -1;
    int i_end = (x == COL - 1) ? 1 : 2;
    int j_end = (y == ROW - 1) ? 1 : 2;

    for (int i = i_start; i < i_end; i++) {
        for (int j = j_start; j < j_end; j++) {
            if (((j + 1) * 3 + (i + 1)) != 4) {
                char tile = grid[(y + j) * COL + (x + i)];
                if (tile == WALL2) {
                    (*nbWalls)++;
                    *house = 1;
                    adjacentTiles[(j + 1) * 3 + (i + 1)] = 2;
                } else if (tile == WALL) {
                    (*nbWalls)++;
                    adjacentTiles[(j + 1) * 3 + (i + 1)] = 1;
                } else {
                    adjacentTiles[(j + 1) * 3 + (i + 1)] = 0;
                }
            }
            else {
                adjacentTiles[(j + 1) * 3 + (i + 1)] = -1;
                if (grid[(y + j) * COL + (x + i)] == WALL2)
                    *house = 1;
            }
        }
    }
    return adjacentTiles;
}


int chooseTile(char* tiles, int nbWalls, int house){
    //Choose the correct png to use depending on the adjacent tiles
    switch(nbWalls){
        case 1:
            if (house)
                return -1;
            if (tiles[1])
                return 11;
            else if (tiles[3])
                return 12;
            else if (tiles[5])
                return 13;
            else if (tiles[7])
                return 14;
            else
                return -1;
            break;
        case 2:
            // Two cases if the house is adjacent to the tile
            if (tiles[3] == 2 && tiles[5])
                return 13;
            if (tiles[5] == 2 && tiles[3])
                return 12;

            if(tiles[0]){
                if (tiles[1])
                    return 11;
                else if(tiles[3])
                    return 12;
                else
                    return -1;
            }
            if (tiles[1]){
                if (tiles[2])
                    return 11;
                else if (tiles[3])
                    return 6;
                else if (tiles[5])
                    return 5;
                else if (tiles[7])
                    return 15;
                else
                    return -1;
            }
            if (tiles[2]){
                if (tiles[5])
                    return 13;
                else
                    return -1;
            }
            if (tiles[3]){
                if (tiles[5])
                    if (house) // case if the tile is the house tile
                        return 10;
                    else
                        return 9;
                else if (tiles[6])
                    return 12;
                else if (tiles[7])
                    return 8;
                else
                    return -1;
            }
            if (tiles[5]){
                if (tiles[7])
                    return 7;
                else if (tiles[8])
                    return 13;
                else
                    return -1;
            }
            if (tiles[7]){
                if (tiles[6] || tiles[8])
                    return 14;
                else
                    return -1;
            }
            break;
        case 3:
            if (house)
                return -1;
            if (tiles[0]){
                if (tiles[1]){
                    if (tiles[2])
                        return 11;
                    else if (tiles[3])
                        return 6;
                    else if (tiles[5])
                        return 5;
                    else if (tiles[7])
                        return 15;
                    else
                        return -1;
                }
                else if (tiles[3]){
                    if (tiles[5])
                        return 9;
                    else if (tiles[6])
                        return 12;
                    else if (tiles[7])
                        return 8;
                    else
                        return -1;
                }
                else
                    return -1;
            }
            if (tiles[1]){
                if (tiles[2]){
                    if (tiles[3])
                        return 6;
                    else if (tiles[5])
                        return 5;
                    else if (tiles[7])
                        return 15;
                    else
                        return -1;
                }
                else if (tiles[3]){
                    if (tiles[5])
                        return 4;
                    else if (tiles[6])
                        return 6;
                    else if (tiles[7])
                        return 2;
                    else
                        return -1;
                }
                else if (tiles[5]){
                    if (tiles[7])
                        return 3;
                    else if (tiles[8])
                        return 5;
                    else
                        return -1;
                }
                else if ((tiles[6] && tiles[7]) || (tiles[7] && tiles[8]))
                    return 15;
                else
                    return -1;
            }
            if (tiles[2]){
                if (tiles[3] && tiles[5])
                    return 9;
                else if (tiles[5] && tiles[7])
                    return 7;
                else if (tiles[5] && tiles[8])
                    return 13;
                else
                    return -1;
            }
            if (tiles[3]){
                if (tiles[5]){
                    if (tiles[6])
                        return 9;
                    else if (tiles[7])
                        return 1;
                    else if (tiles[8])
                        return 9;
                }
                else if ((tiles[6] && tiles[7]) || (tiles[7] && tiles[8]))
                    return 8;
                else
                    return -1;
            }
            if (tiles[5]){
                if ((tiles[6] && tiles[7]) || (tiles[7] && tiles[8]))
                    return 7;
                else
                    return -1;
            }
            if (tiles[6] && tiles[7] && tiles[8])
                return 14;
            else
                return -1;
            break;
        case 4:
            if (house)
                return -1;
            if (tiles[0]){
                if (tiles[1]){
                    if (tiles[2]){
                        if (tiles[3])
                            return 6;
                        else if (tiles[5])
                            return 5;
                        else if (tiles[7])
                            return 15;
                        else
                            return -1;
                    }
                    else if (tiles[3]){
                        if (tiles[5])
                            return 4;
                        else if (tiles[6])
                            return 6;
                        else if (tiles[7])
                            return 2;
                        else
                            return -1;
                    }
                    else if (tiles[5]){
                        if (tiles[7])
                            return 3;
                        else if (tiles[8])
                            return 5;
                        else
                            return -1;
                    }
                    else if ((tiles[7] && tiles[8]) || (tiles[6] && tiles[7]))
                        return 15;
                    else
                        return -1;
                }
                else if (tiles[3]){
                    if (tiles[5]){
                        if (tiles[6])
                            return 9;
                        else if (tiles[7])
                            return 1;
                        else if (tiles[8])
                            return 9;
                        else
                            return -1;
                    }
                    else if ((tiles[6] && tiles[7]) || (tiles[7] && tiles[8]))
                        return 8;
                    else
                        return -1;
                }
                else
                    return -1;
            }
            else if (tiles[1]){
                if (tiles[2]){
                    if (tiles[3]){
                        if (tiles[5])
                            return 4;
                        else if (tiles[6])
                            return 6;
                        else if (tiles[7])
                            return 2;
                        else
                            return -1;
                    }
                    else if (tiles[5]){
                        if (tiles[7])
                            return 3;
                        else if (tiles[8])
                            return 5;
                        else
                            return -1;
                    }
                    else if ((tiles[6] && tiles[7]) || (tiles[7] && tiles[8]))
                        return 15;
                    else
                        return -1;
                }
                else if (tiles[3]){
                    if (tiles[5]){
                        if (tiles[6])
                            return 4;
                        else if (tiles[7])
                            return 16; // special case crossroads
                        else if (tiles[8])
                            return 4;
                        else
                            return -1;
                    }
                    else if ((tiles[6] && tiles[7]) || (tiles[7] && tiles[8]))
                        return 2;
                }
                else if (tiles[5]){
                    if ((tiles[6] && tiles[7]) || (tiles[7] && tiles[8]))
                        return 3;
                    else
                        return -1;
                }
                else if (tiles[6] && tiles[7] && tiles[8])
                    return 15;
                else
                    return -1;
            }
            else if (tiles[2] && tiles[3] && tiles[5] && tiles[6])
                return 9;
            else if (tiles[2] && tiles[3] && tiles[5] && tiles[7])
                return 1;
            else if (tiles[2] && tiles[3] && tiles[5] && tiles[8])
                return 9;
            else if (tiles[3]){
                if (tiles[5]){
                    if (tiles[6]){
                        if (tiles[7])
                            return 1;
                        else
                            return -1;
                    }
                    else if (tiles[7] && tiles[8])
                        return 1;
                    else
                        return -1;
                }
                else if (tiles[6] && tiles[7] && tiles[8])
                    return 8;
                else
                    return -1;
            }
            else if (tiles[5] && tiles[6] && tiles[7] && tiles[8])
                return 7;
            else
                return -1;
            break;
        case 5:
            if (house)
                return -1;
            if (tiles[0]){
                if (tiles[1]){
                    if (tiles[2]){
                        if (tiles[3]){
                            if (tiles[5])
                                return 9;
                            else if (tiles[6])
                                return 6;
                            else if (tiles[7])
                                return 2;
                            else
                                return -1;
                        }
                        else if (tiles[5]){
                            if (tiles[7])
                                return 3;
                            else if (tiles[8])
                                return 5;
                            else
                                return -1;
                        }
                        else
                            return -1;
                    }
                    else if (tiles[3]){
                        if (tiles[5]){
                            if (tiles[6])
                                return 4;
                            else if (tiles[7])
                                return 16; // special case crossroads
                            else if (tiles[8])
                                return 4;
                            else
                                return -1;
                        }
                        else if (tiles[6] && tiles[7])
                            return 15;
                        else if (tiles[7] && tiles[8])
                            return 3;
                        else
                            return -1;
                    }
                    else if (tiles[5]){
                        if (tiles[6] && tiles[7])
                            return 2;
                        else if (tiles[7] && tiles[8])
                            return 15;
                        else
                            return -1;
                    }
                    else if (tiles[6] && tiles[7] && tiles[8])
                        return 15;
                    else
                        return -1;
                }
                else if (tiles[3]){
                    if (tiles[5]){
                        if (tiles[6]){
                            if (tiles[7])
                                return 1;
                            else
                                return -1;
                        }
                        else if (tiles[7] && tiles[8])
                            return 1;
                        else
                            return -1;
                    }
                    else if (tiles[6] && tiles[7] && tiles[8])
                        return 8;
                    else
                        return -1;
                }
                else 
                    return -1;
            }
            else if (tiles[1]){
                if (tiles[2]){
                    if (tiles[3]){
                        if (tiles[5]){
                            if (tiles[6])
                                return 4;
                            else if (tiles[7])
                                return 16; // special case crossroads
                            else if (tiles[8])
                                return 4;
                            else
                                return -1;
                        }
                        else if (tiles[6]){
                            if (tiles[7])
                                return 2;
                            else
                                return -1;
                        }
                        else
                            return -1;
                    }
                    else if (tiles[5]){
                        if (tiles[6] && tiles[7])
                            return 3;
                        else if (tiles[7] && tiles[8])
                            return 15;
                        else
                            return -1;
                    }
                    else
                        return -1;
                }
                else if (tiles[3]){
                    if (tiles[5]){
                        if (tiles[6] && tiles[7])
                            return 16; // special case crossroads
                        else if (tiles[7] && tiles[8])
                            return 16; // special case crossroads
                        else
                            return -1;
                    }
                    else if (tiles[6] && tiles[7] && tiles[8])
                        return 2;
                    else
                        return -1;
                }
                else if (tiles[5] && tiles[6] && tiles[7] && tiles[8])
                    return 3;
                else
                    return -1;
            }
            else if (tiles[2]){
                if (tiles[3]){
                    if (tiles[5]){
                        if (tiles[6]){
                            if (tiles[7])
                                return 1;
                            else if (tiles[8])
                                return 9;
                            else
                                return -1;
                        }
                        else if (tiles[7] && tiles[8])
                            return 1;
                        else
                            return -1;
                    }
                    else 
                        return -1;
                }
                else if (tiles[5]){
                    if (tiles[6] && tiles[7] && tiles[8])
                        return 7;
                    else
                        return -1;
                }
                else
                    return -1;
            }
            else if (tiles[3]){
                if (tiles[5]){
                    if (tiles[6] && tiles[7] && tiles[8])
                        return 9;
                    else
                        return -1;
                }
                else
                    return -1;
            }
            else
                return -1;
            break;
        case 6:
            if (house)
                return -1;
            if (!tiles[0]){
                if (!tiles[1])
                    return 9;
                else if (!tiles[2])
                    return 4;
                else if (!tiles[3])
                    return 15;
                else if (!tiles[6])
                    return 2;
                else if (!tiles[8])
                    return 16; // special case crossroads
                else
                    return -1;
            }
            else if (!tiles[1] && !tiles[2])
                return 9;
            else if (!tiles[2]){
                if (!tiles[5])
                    return 15;
                else if (!tiles[6])
                    return 16;
                else if (!tiles[8])
                    return 3;
                else
                    return -1;
            }
            else if (!tiles[3] && !tiles[6])
                return 15;
            else if (!tiles[5] && !tiles[8])
                return 15;
            else if (!tiles[6]){
                if (!tiles[7])
                    return 9;
                else if (!tiles[8])
                    return 1;
                else
                    return -1;
            }
            else if (!tiles[7] && !tiles[8])
                return 9;
            else
                return -1;
            break;
        case 7:
            if (house)
                return -1;
            if (!tiles[0])
                return 6;
            else if (!tiles[1] || !tiles[7])
                return 9;
            else if (!tiles[2])
                return 5;
            else if (!tiles[3] || !tiles[5])
                return 15;
            else if (!tiles[6])
                return 8;
            else if (!tiles[8])
                return 7;
            else
                return -1;
            break;
        case 8:
            if (house)
                return -1;
            return 0;
            break;
    }
    return -1;
}

void combinePNGs(const char* outputFilename, char* map)
{
    char* grid = map;

    // Initialize SDL's Video subsystem
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("Image Processing",
            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, 0);
    
    // Load the image into memory using SDL_image library function
    SDL_Surface* VerticalWall = IMG_Load("./tiles/VerticalWall.png");
    SDL_Surface* HorizontalWall = IMG_Load("./tiles/HorizontalWall.png");
    SDL_Surface* CornerBottomLeft = IMG_Load("./tiles/CornerBottomLeft.png");
    SDL_Surface* CornerBottomRight = IMG_Load("./tiles/CornerBottomRight.png");
    SDL_Surface* CornerTopLeft = IMG_Load("./tiles/CornerTopLeft.png");
    SDL_Surface* CornerTopRight = IMG_Load("./tiles/CornerTopRight.png");
    SDL_Surface* UCornerBottom = IMG_Load("./tiles/UCornerBottom.png");
    SDL_Surface* UCornerLeft = IMG_Load("./tiles/UCornerLeft.png");
    SDL_Surface* UCornerRight = IMG_Load("./tiles/UCornerRight.png");
    SDL_Surface* UCornerTop = IMG_Load("./tiles/UCornerTop.png");
    SDL_Surface* HouseWall = IMG_Load("./tiles/HouseWall.png");
    SDL_Surface* DIntersectionTop = IMG_Load("./tiles/1DIntersectionTop.png");
    SDL_Surface* DIntersectionBottom = IMG_Load("./tiles/1DIntersectionBottom.png");
    SDL_Surface* DIntersectionLeft = IMG_Load("./tiles/1DIntersectionLeft.png");
    SDL_Surface* DIntersectionRight = IMG_Load("./tiles/1DIntersectionRight.png");

    // Get the width and height of the surface to be created
    int combinedWidth = COL * SPRITE_SIZE;
    int combinedHeight = ROW * SPRITE_SIZE;

    SDL_Surface* combinedImage = SDL_CreateRGBSurface(0, combinedWidth,
            combinedHeight, 32, 0, 0, 0, 0);

    SDL_Rect srcRect = {0, 0, SPRITE_SIZE, SPRITE_SIZE};
    SDL_Rect dstRect = {0, 0, 0, 0};
    int globalX = 0;
    int globalY = 0;
    //Go through the map and assign the correct sprite at the correct location
    // -> loop through each row
    // -> loop through each column of the row
    //    -> check what block to put if it is a wall (WALL || WALL2)
    //      . Look in the 3*3 adjacent tiles:
    //          _ Depending on the adjacent tiles, select the correct image to
    //            use
    //      . Use SDL_BlitSurface to copy the right PNG to the right spot
    // Once out of the loop save the combined image to the output file name
    int* nbWalls = malloc(sizeof(int));
    for (int y = 0; y < ROW; y++){
        for (int x = 0; x < COL; x++){
            if (grid[y * COL + x] == WALL || grid[y * COL + x] == WALL2){
                int house = 0;
                char *adjacentTiles = nbAdjacentWalls(grid, x, y, nbWalls,
                        &house);
                printf("x: %d, y: %d\n", x, y);
                if(adjacentTiles[0])
                    printf("X");
                else
                    printf("O");
                if(adjacentTiles[1])
                    printf("X");
                else
                    printf("O");
                if(adjacentTiles[2])
                    printf("X");
                else
                    printf("O");
                printf("\n");
                if(adjacentTiles[3])
                    printf("X");
                else
                    printf("O");
                printf("X");
                if(adjacentTiles[5])
                    printf("X");
                else
                    printf("O");
                printf("\n");
                if(adjacentTiles[6])
                    printf("X");
                else
                    printf("O");
                if(adjacentTiles[7])
                    printf("X");
                else
                    printf("O");
                if(adjacentTiles[8])
                    printf("X");
                else
                    printf("O");
                printf("\n");
                int res = chooseTile(adjacentTiles, *nbWalls, house);
                switch(res){
                    case 0: // if case zero == empty tile, increment globalX
                        globalX += SPRITE_SIZE;
                        break;
                    case 1:
                        dstRect.x = globalX;
                        dstRect.y = globalY;
                        dstRect.w = SPRITE_SIZE;
                        dstRect.h = SPRITE_SIZE;
                        SDL_BlitSurface(DIntersectionBottom,
                                &srcRect, combinedImage, &dstRect);
                        globalX += SPRITE_SIZE;
                        break;
                    case 2:
                        dstRect.x = globalX;
                        dstRect.y = globalY;
                        dstRect.w = SPRITE_SIZE;
                        dstRect.h = SPRITE_SIZE;
                        SDL_BlitSurface(DIntersectionLeft,
                                &srcRect, combinedImage, &dstRect);
                        globalX += SPRITE_SIZE;
                        break;
                    case 3:
                        dstRect.x = globalX;
                        dstRect.y = globalY;
                        dstRect.w = SPRITE_SIZE;
                        dstRect.h = SPRITE_SIZE;
                        SDL_BlitSurface(DIntersectionRight,
                                &srcRect, combinedImage, &dstRect);
                        globalX += SPRITE_SIZE;
                        break;
                    case 4:
                        dstRect.x = globalX;
                        dstRect.y = globalY;
                        dstRect.w = SPRITE_SIZE;
                        dstRect.h = SPRITE_SIZE;
                        SDL_BlitSurface(DIntersectionTop,
                                &srcRect, combinedImage, &dstRect);
                        globalX += SPRITE_SIZE;
                        break;
                    case 5:
                        dstRect.x = globalX;
                        dstRect.y = globalY;
                        dstRect.w = SPRITE_SIZE;
                        dstRect.h = SPRITE_SIZE;
                        SDL_BlitSurface(CornerBottomLeft,
                                &srcRect, combinedImage, &dstRect);
                        globalX += SPRITE_SIZE;
                        break;
                    case 6:
                        dstRect.x = globalX;
                        dstRect.y = globalY;
                        dstRect.w = SPRITE_SIZE;
                        dstRect.h = SPRITE_SIZE;
                        SDL_BlitSurface(CornerBottomRight,
                                &srcRect, combinedImage, &dstRect);
                        globalX += SPRITE_SIZE;
                        break;
                    case 7:
                        dstRect.x = globalX;
                        dstRect.y = globalY;
                        dstRect.w = SPRITE_SIZE;
                        dstRect.h = SPRITE_SIZE;
                        SDL_BlitSurface(CornerTopLeft,
                                &srcRect, combinedImage, &dstRect);
                        globalX += SPRITE_SIZE;
                        break;
                    case 8:
                        dstRect.x = globalX;
                        dstRect.y = globalY;
                        dstRect.w = SPRITE_SIZE;
                        dstRect.h = SPRITE_SIZE;
                        SDL_BlitSurface(CornerTopRight,
                                &srcRect, combinedImage, &dstRect);
                        globalX += SPRITE_SIZE;
                        break;
                    case 9:
                        dstRect.x = globalX;
                        dstRect.y = globalY;
                        dstRect.w = SPRITE_SIZE;
                        dstRect.h = SPRITE_SIZE;
                        SDL_BlitSurface(HorizontalWall,
                                &srcRect, combinedImage, &dstRect);
                        globalX += SPRITE_SIZE;
                        break;
                    case 10:
                        dstRect.x = globalX;
                        dstRect.y = globalY;
                        dstRect.w = SPRITE_SIZE;
                        dstRect.h = SPRITE_SIZE;
                        SDL_BlitSurface(HouseWall,
                                &srcRect, combinedImage, &dstRect);
                        globalX += SPRITE_SIZE;
                        break;
                    case 11:
                        dstRect.x = globalX;
                        dstRect.y = globalY;
                        dstRect.w = SPRITE_SIZE;
                        dstRect.h = SPRITE_SIZE;
                        SDL_BlitSurface(UCornerBottom,
                                &srcRect, combinedImage, &dstRect);
                        globalX += SPRITE_SIZE;
                        break;
                    case 12:
                        dstRect.x = globalX;
                        dstRect.y = globalY;
                        dstRect.w = SPRITE_SIZE;
                        dstRect.h = SPRITE_SIZE; 
                        SDL_BlitSurface(UCornerRight,
                                &srcRect, combinedImage, &dstRect);
                        globalX += SPRITE_SIZE;
                        break;
                    case 13:
                        dstRect.x = globalX;
                        dstRect.y = globalY;
                        dstRect.w = SPRITE_SIZE;
                        dstRect.h = SPRITE_SIZE; 
                        SDL_BlitSurface(UCornerLeft,
                                &srcRect, combinedImage, &dstRect);
                        globalX += SPRITE_SIZE;
                        break;
                    case 14:
                        dstRect.x = globalX;
                        dstRect.y = globalY;
                        dstRect.w = SPRITE_SIZE;
                        dstRect.h = SPRITE_SIZE; 
                        SDL_BlitSurface(UCornerTop,
                                &srcRect, combinedImage, &dstRect);
                        globalX += SPRITE_SIZE;
                        break;
                    case 15:
                        dstRect.x = globalX;
                        dstRect.y = globalY;
                        dstRect.w = SPRITE_SIZE;
                        dstRect.h = SPRITE_SIZE; 
                        SDL_BlitSurface(VerticalWall,
                                &srcRect, combinedImage, &dstRect);
                        globalX += SPRITE_SIZE;
                        break;
                    /*case 16:
                     * TODO: implement this
                        //not implemented yet 
                        //Cross intersection
                        dstRect.x = globalX;
                        dstRect.y = globalY;
                        dstRect.w = SPRITE_SIZE;
                        dstRect.h = SPRITE_SIZE;
                        SDL_BlitSurface(CrossIntersection,
                                &srcRect, combinedImage, &dstRect);
                        globalX += SPRITE_SIZE;
                        break;*/
                    case -1:
                        // error not a valid tile
                        printf("Invalid tile at %d, %d\n", x, y);

                        // print the adjacent tiles array
                        for (int i = 0; i < 9; i++){
                            printf("%d ", adjacentTiles[i]);
                        }
                        printf("\n");
                        errx(1, "Invalid tile");
                        break;
                }
                
                free(adjacentTiles);
            }
            else
                globalX += SPRITE_SIZE;
            
            if (globalX >= combinedWidth){
                    globalX = 0;
                    globalY += SPRITE_SIZE;
            }
        }
    }
    free(nbWalls);
    // Save the image in png format
    IMG_SavePNG(combinedImage, outputFilename);

    // Cleanup
    SDL_FreeSurface(VerticalWall);
    SDL_FreeSurface(HorizontalWall);
    SDL_FreeSurface(CornerBottomLeft);
    SDL_FreeSurface(CornerBottomRight);
    SDL_FreeSurface(CornerTopRight);
    SDL_FreeSurface(CornerTopLeft);
    SDL_FreeSurface(UCornerBottom);
    SDL_FreeSurface(UCornerLeft);
    SDL_FreeSurface(UCornerRight);
    SDL_FreeSurface(UCornerTop);
    SDL_FreeSurface(HouseWall);
    SDL_FreeSurface(DIntersectionLeft);
    SDL_FreeSurface(DIntersectionRight);
    SDL_FreeSurface(DIntersectionTop);
    SDL_FreeSurface(DIntersectionBottom);

    SDL_FreeSurface(combinedImage);
    
    SDL_DestroyWindow(window);
    
    SDL_Quit();
}
