#include <stdlib.h>
#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>  
#include "mapgen/txtToPNG.h"

#include "main.h"
#include "pacman_ai/search.h"

#define TILE_SIZE 32
enum{HAUT,BAS,GAUCHE,DROITE};

int score = 0;
int lives = 0;
int game_won;
int image_width, image_height;

int get_input();
int all_eaten(Game* game);
void draw_final(SDL_Renderer* renderer);

int msleep(long msec)
{
    struct timespec ts;
    int res;

    if (msec < 0)
    {
        errno = EINVAL;
        return -1;
    }

    ts.tv_sec = msec / 1000;
    ts.tv_nsec = (msec % 1000) * 1000000;

    do {
        res = nanosleep(&ts, &ts);
    } while (res && errno == EINTR);

    return res;
}


void draw_save(SDL_Renderer* renderer,Game* game)
{
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer,11,63,114,255);
    SDL_Surface* save_surface = IMG_Load("menuimg/save_map.png");
    SDL_Texture* save_texture = SDL_CreateTextureFromSurface(renderer, save_surface);
    SDL_FreeSurface(save_surface);

    SDL_QueryTexture(save_texture, NULL, NULL, &image_width, &image_height);
    SDL_Rect save_rect = { 0, 0, 1200, 928};
    
    SDL_RenderCopy(renderer, save_texture, NULL, &save_rect);

    int buttonwidth = 371;
    int buttonheight = 351;

    int yes_y = 325;
    int yes_x = 138;
    int no_y = 323;
    int no_x = 688;


    SDL_RenderPresent(renderer);

    SDL_Event event;
    while (SDL_WaitEvent(&event)) {
        
        if (event.type == SDL_QUIT) {
            SDL_DestroyTexture(save_texture);
            SDL_DestroyRenderer(renderer);
            TTF_Quit();
            IMG_Quit();
            SDL_Quit();
            exit(0);
        } 
        else if (event.type == SDL_MOUSEBUTTONDOWN) {
            int x = event.button.x;
            int y = event.button.y;
            if (x >= yes_x && x <= yes_x + buttonwidth &&
                y >= yes_y && y <= yes_y + buttonheight) {
                break;
            }
            else if (x >= no_x && x <= no_x + buttonwidth &&
             y >= no_y && y <= no_y + buttonheight) {
                game->map->points+=0; //just for testing purposes
                /*
                char name[128];
                // function to retrieve input user
                save_map(game->map, name);*/
                break;
            }
        }
    }
    SDL_DestroyTexture(save_texture);
}

void draw_Ghost(SDL_Renderer* renderer,Game* game,SDL_Rect* position)
{
    SDL_Surface *blinky[4]={NULL};
    SDL_Surface *blinkyActuel = NULL;
    blinky[HAUT]= IMG_Load("characters/blinky_up.png");
    blinky[BAS]= IMG_Load("characters/blinky_down.png");
    blinky[GAUCHE]= IMG_Load("characters/blinky_left.png");
    blinky[DROITE]= IMG_Load("characters/blinky_right.png");
    SDL_Texture* blinky_texture = SDL_CreateTextureFromSurface(renderer, blinkyActuel);

    SDL_Surface *clyde[4]={NULL};
    SDL_Surface *clydeActuel = NULL;

    clyde[HAUT]= IMG_Load("characters/clyde_up.png");
    clyde[BAS]= IMG_Load("characters/clyde_down.png");
    clyde[GAUCHE]= IMG_Load("characters/clyde_left.png");
    clyde[DROITE]= IMG_Load("characters/clyde_right.png");
    SDL_Texture* clyde_texture = SDL_CreateTextureFromSurface(renderer, clydeActuel);

    SDL_Surface *pinky[4]={NULL};
    SDL_Surface *pinkyActuel = NULL;
    pinky[HAUT]= IMG_Load("characters/pinky_up.png");
    pinky[BAS]= IMG_Load("characters/pinky_down.png");
    pinky[GAUCHE]= IMG_Load("characters/pinky_left.png");
    pinky[DROITE]= IMG_Load("characters/pinky_right.png");

    SDL_Surface *inky[4]={NULL};
    SDL_Surface *inkyActuel = NULL;
    inky[HAUT]= IMG_Load("characters/inky_up.png");
    inky[BAS]= IMG_Load("characters/inky_down.png");
    inky[GAUCHE]= IMG_Load("characters/inky_left.png");
    inky[DROITE]= IMG_Load("characters/inky_right.png");

    SDL_Texture* inky_texture = SDL_CreateTextureFromSurface(renderer, inkyActuel);
    SDL_Texture* pinky_texture = SDL_CreateTextureFromSurface(renderer, pinkyActuel);

   
    SDL_Surface* fghost = IMG_Load("characters/fghost.png");
    SDL_Surface* dghost = IMG_Load("characters/dghost.png");

    for(size_t i = 0; i < game->ghosts->length;i++){
        Ghost* ghost = llist_use(game->ghosts, i);
        switch (ghost->mode)
        {
            case DEAD:
                position->x= (ghost->x*TILE_SIZE);
                position->y= (ghost->y* TILE_SIZE);
                if (i == 0){
                    blinkyActuel= dghost;
                    blinky_texture = SDL_CreateTextureFromSurface(renderer, blinkyActuel);
                    SDL_RenderCopy(renderer, blinky_texture, NULL, position);
                }
                else if (i == 1){
                    clydeActuel= dghost;
                    clyde_texture = SDL_CreateTextureFromSurface(renderer, clydeActuel);
                    SDL_RenderCopy(renderer, clyde_texture, NULL, position);
                }
                else if (i == 2){
                    pinkyActuel= dghost;
                    pinky_texture = SDL_CreateTextureFromSurface(renderer, pinkyActuel);
                    SDL_RenderCopy(renderer, pinky_texture, NULL, position);
                }
                else if (i == 3){
                    inkyActuel= dghost;
                    inky_texture = SDL_CreateTextureFromSurface(renderer, inkyActuel);
                    SDL_RenderCopy(renderer, inky_texture, NULL, position);
                }
                break;
            case FRIGHTENED:
                position->x= (ghost->x*TILE_SIZE);
                position->y= (ghost->y* TILE_SIZE);
                if (i == 0){
                    blinkyActuel= fghost;
                    blinky_texture = SDL_CreateTextureFromSurface(renderer, blinkyActuel);
                    SDL_RenderCopy(renderer, blinky_texture, NULL, position);
                }
                else if (i == 1){
                    clydeActuel= fghost;
                    clyde_texture = SDL_CreateTextureFromSurface(renderer, clydeActuel);
                    SDL_RenderCopy(renderer, clyde_texture, NULL, position);
                }
                else if (i == 2){
                    pinkyActuel= fghost;
                    pinky_texture = SDL_CreateTextureFromSurface(renderer, pinkyActuel);
                    SDL_RenderCopy(renderer, pinky_texture, NULL, position);
                }
                else if (i == 3){
                    inkyActuel= fghost;
                    inky_texture = SDL_CreateTextureFromSurface(renderer, inkyActuel);
                    SDL_RenderCopy(renderer, inky_texture, NULL, position);
                }
                break;
            default:
                if (i == 0)
                {
                    position->x= (ghost->x*TILE_SIZE);
                    position->y= (ghost->y* TILE_SIZE);
                    blinkyActuel=blinky[ghost->direction];
                    blinky_texture = SDL_CreateTextureFromSurface(renderer, blinkyActuel);
                    SDL_RenderCopy(renderer, blinky_texture, NULL, position);
                }
                else if (i == 1)
                {
                    position->x= (ghost->x*TILE_SIZE);
                    position->y= (ghost->y* TILE_SIZE);
                    clydeActuel=clyde[ghost->direction];
                    clyde_texture = SDL_CreateTextureFromSurface(renderer, clydeActuel);
                    SDL_RenderCopy(renderer, clyde_texture, NULL, position);
                }
                else if (i == 2)
                {
                    position->x= (ghost->x*TILE_SIZE);
                    position->y= (ghost->y* TILE_SIZE);
                    pinkyActuel=pinky[ghost->direction];
                    pinky_texture = SDL_CreateTextureFromSurface(renderer, pinkyActuel);
                    SDL_RenderCopy(renderer, pinky_texture, NULL, position);
                }
                else if (i == 3)
                {
                    position->x= (ghost->x*TILE_SIZE);
                    position->y= (ghost->y* TILE_SIZE);
                    inkyActuel=inky[ghost->direction];
                    inky_texture = SDL_CreateTextureFromSurface(renderer, inkyActuel);
                    SDL_RenderCopy(renderer, inky_texture, NULL, position);
                }
                break;
        }
    }
  
    for (int i =0;i<4;i++)
    {
        SDL_FreeSurface(blinky[i]);
        SDL_FreeSurface(clyde[i]);
        SDL_FreeSurface(inky[i]);
        SDL_FreeSurface(pinky[i]);
    }
    SDL_FreeSurface(fghost);
    SDL_FreeSurface(dghost);
}

void Move(SDL_Rect* pos,int direction)
{
    switch (direction)
    {
    case HAUT:
        pos->y--;
        break;
    case BAS:
        pos->y++;
        break;
    case GAUCHE:
        pos->x--;
        break;
    case DROITE:
        pos->x++;
        break;
    
    default:
        break;
    }
} 

void drawScore(SDL_Renderer* renderer,Game* game)
{
    
    char scoreText[20];
    score = game->map->points;
    sprintf(scoreText, "%d", score);
    TTF_Font* font = TTF_OpenFont("Paperkind.ttf", 65);
    SDL_Color text_color = { 255, 255,255, 255 };
    SDL_Surface* scoreSurface = TTF_RenderText_Solid(font, scoreText, text_color);
    SDL_Texture* scoreTexture = SDL_CreateTextureFromSurface(renderer, scoreSurface);
    SDL_Rect scoreTextRect = { 950, 300, scoreSurface->w, scoreSurface->h };
    SDL_RenderCopy(renderer, scoreTexture, NULL, &scoreTextRect);
    SDL_FreeSurface(scoreSurface);
    SDL_DestroyTexture(scoreTexture);
    TTF_CloseFont(font);
}

void drawLives(SDL_Renderer* renderer,Game* game)
{
    char scoreText[20];
    lives = game->pacman->lives;
    sprintf(scoreText, "%d", lives);
    TTF_Font* font = TTF_OpenFont("Paperkind.ttf", 65);
    SDL_Color text_color = { 255, 255,255, 255 };
    SDL_Surface* scoreSurface = TTF_RenderText_Solid(font, scoreText, text_color);
    SDL_Texture* scoreTexture = SDL_CreateTextureFromSurface(renderer, scoreSurface);
    SDL_Rect scoreTextRect = { 1005, 575, scoreSurface->w, scoreSurface->h };
    SDL_RenderCopy(renderer, scoreTexture, NULL, &scoreTextRect);
    SDL_FreeSurface(scoreSurface);
    SDL_DestroyTexture(scoreTexture);
    TTF_CloseFont(font);
}


void draw_game(SDL_Renderer* renderer,Game* game,int map_load)
{
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_Surface* map_surface= IMG_Load("Default_Map.png");
    
    if (map_load == 0)
    {
        char* path =(char*)malloc(20 * sizeof(char));
        sprintf(path, "png/map%d.png", game->map->num);
        map_surface= IMG_Load(path);
        free(path);
    }
    else
    {
        char* path =(char*)malloc(20 * sizeof(char));
        sprintf(path, "png/map%d.png", game->map->num);
        map_surface= IMG_Load(path);
        free(path);   
    }
    SDL_Texture* map_texture = SDL_CreateTextureFromSurface(renderer, map_surface);
    SDL_FreeSurface(map_surface);

    int map_x = 0;
    int map_y = 0;
    int map_height = 864;
    int map_width = 928 ;
    
    SDL_Rect map_rect = { map_x, map_y, map_height,map_width};
    SDL_RenderCopy(renderer, map_texture, NULL, &map_rect);
 
    SDL_Surface *pacman[4]={NULL};
    SDL_Surface *pacmanActuel = NULL;

    pacman[HAUT]= IMG_Load("characters/pacman_up.png");
    pacman[BAS]= IMG_Load("characters/pacman_down.png");
    pacman[GAUCHE]= IMG_Load("characters/pacman_left.png");
    pacman[DROITE]= IMG_Load("characters/pacman_right.png");

    pacmanActuel= pacman[DROITE];
    SDL_Texture* pac_texture = SDL_CreateTextureFromSurface(renderer, pacmanActuel);
    int pac_x = game->pacman->x*TILE_SIZE;
    int pac_y = game->pacman->y*TILE_SIZE;
    int pac_height = 32 ;
    int pac_width =  32;
    SDL_Rect position = {pac_x,pac_y,pac_height,pac_width};

    SDL_RenderCopy(renderer, pac_texture, NULL, &position);
    SDL_RenderPresent(renderer);

    SDL_Surface* pp = IMG_Load("characters/power_pellet.png");
    SDL_Texture* pp_texture = SDL_CreateTextureFromSurface(renderer, pp);
    SDL_FreeSurface(pp);
    SDL_Surface* tt = IMG_Load("characters/tic_tac.png");
    SDL_Texture* tt_texture = SDL_CreateTextureFromSurface(renderer, tt);
    SDL_FreeSurface(tt);

    int countdown = 10;
    int state = 0; 
    int pos;  
    char c; 

    SDL_Surface* bg_surface= IMG_Load("menuimg/bg.png");
    SDL_Texture* bg_texture = SDL_CreateTextureFromSurface(renderer, bg_surface);
    SDL_FreeSurface(bg_surface);
    SDL_Rect bg_rect = { 0, 0, 1200,928};

    score =0;

    while (game_over(game) == 0 && all_eaten(game) == 1){
        
        SDL_RenderCopy(renderer, bg_texture, NULL, &bg_rect);
        SDL_RenderCopy(renderer, map_texture, NULL, &map_rect);
        drawScore(renderer,game);
        drawLives(renderer,game);
        
        for (int i = 0;i<COL;i++)
        {
            for(int j=0;j<ROW;j++)
            {
                position.x=i*TILE_SIZE;
                position.y= j*TILE_SIZE;
                c=game->map->grid[i+j*COL];
                switch(c)
                {
                    case '.':
                        SDL_RenderCopy(renderer, tt_texture, NULL, &position);

                        break;
                    case 'o':
                        SDL_RenderCopy(renderer, pp_texture, NULL, &position);
                        break;
                    
                    default:
                        break;

                }

            }
        } 
        if(game->difficulty>0)
        {
            draw_Ghost(renderer,game,&position);
        } 
        position.x= (game->pacman->x*TILE_SIZE);
        position.y= (game->pacman->y*TILE_SIZE); 
        pac_texture = SDL_CreateTextureFromSurface(renderer, pacmanActuel);
        SDL_RenderCopy(renderer, pac_texture, NULL, &position);
    
        SDL_RenderPresent(renderer);  
        
        if (game->is_ai != 1){
            pos = game->pacman->x + game->pacman->y * COL;
            SDL_Event event;
            while (SDL_PollEvent(&event)) {
    
                if (event.type == SDL_QUIT) {
                    SDL_DestroyTexture(map_texture);
                    SDL_DestroyRenderer(renderer);
                    TTF_Quit();
                    IMG_Quit();
                    SDL_Quit();
                    exit(0);
                    break;
                } 
                else if (event.type == SDL_KEYDOWN) { 
                    switch (event.key.keysym.sym) {
                        case SDLK_UP:
                            if (game->map->grid[pos + UP] != WALL && game->map->grid[pos + UP] != WALL2){
                                game->pacman->direction = UP; 
                                pacmanActuel = pacman[HAUT];
                                Move(&position,HAUT);
                                

                            }
                            break;
                        case SDLK_DOWN:
                            if (game->map->grid[pos + DOWN] != WALL && game->map->grid[pos + DOWN] != WALL2){
                                game->pacman->direction = DOWN;
                                pacmanActuel = pacman[BAS];
                                Move(&position,BAS);
                                
                            }
                            break;
                        case SDLK_LEFT:
                            if (game->map->grid[pos + LEFT] != WALL && game->map->grid[pos + LEFT] != WALL2){
                                game->pacman->direction = LEFT;
                                pacmanActuel = pacman[GAUCHE];
                                Move(&position,GAUCHE);
                            }
                            break;
                        case SDLK_RIGHT:
                            if (game->map->grid[pos + RIGHT] != WALL && game->map->grid[pos + RIGHT] != WALL2){
                                game->pacman->direction = RIGHT;
                                pacmanActuel = pacman[DROITE];
                                Move(&position,DROITE);
                                
                            }
                            break;
                        default:
                            break;
                    }
                    break;
                }
            }
            state = update(game);
            msleep(200);
            
		}
		else{
            size_t pos = game->pacman->x + game->pacman->y * COL;
            switch (game->difficulty) {
                case PEACEFULL:
                    game->pacman->direction = Bellman(game, pos);
                    break;
                case EASY:
                    game->pacman->direction = Bellman(game, pos);                   
                    break;
                case HARD:
                    game->pacman->direction = Bellman(game, pos);
                    break;
            }
            msleep(100);// need to be modified
            
            state = update(game);
            if (game->pacman->direction == RIGHT)
            {
                pacmanActuel = pacman[DROITE];
                Move(&position,DROITE);
                
            }
            else if (game->pacman->direction == LEFT)
            {
                pacmanActuel = pacman[GAUCHE];
                Move(&position,GAUCHE);
                
            }
            else if (game->pacman->direction == UP)
            {
                pacmanActuel = pacman[HAUT];
                Move(&position,HAUT);
                
            }
            else if (game->pacman->direction == DOWN)
            {
                pacmanActuel = pacman[BAS];
                Move(&position,BAS);
                
            }
		} 
        state = countdown;
        countdown = state;
        if (game->pacman->lives == 0){
            break;
        }       
    }
    score = game->map->points;
    game_won = game->pacman->lives;
    if (map_load==0)
    {
        draw_save(renderer,game);
    }
    for (int i =0;i<4;i++)
    {
        SDL_FreeSurface(pacman[i]);
    }
    free_game(game);
    draw_final(renderer);
    //SDL_DestroyTexture(map_texture);

}
