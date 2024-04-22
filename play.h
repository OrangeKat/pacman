#ifndef PLAY_H
#define PLAY_H

void draw_Ghost(SDL_Renderer* renderer,Game* game,SDL_Rect* position);
void Move(SDL_Rect* pos,int direction);
void draw_game(SDL_Renderer* renderer,Game* game,int map_load);


#endif