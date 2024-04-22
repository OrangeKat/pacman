#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <unistd.h>
#include "main.h"
#include "obj/ghost.h"
#include "obj/player.h"
#include "pacman_ai/lib/linked_list.h"
#include "pacman_ai/minimax.h"
#include "src/map.h"
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <termios.h>
#include "pacman_ai/search.h"
#include "mapgen/txtToPNG.h"
#include "play.h"


#define MAX_MAPS 10
#define MAPS_FOLDER "maps"
#define MAX_FILES 256

char selectedMapPath[512] = "maps/map_1.txt";
char DEFAULT_MAP[] = {"############################............#............##.####.#####.#.#####.####.##o####.#####.#.#####.####o##.####.#####.#.#####.####.##.........................##.####.##.#######.##.####.##.####.##.#######.##.####.##......##....#....##......#######.##### # #####.######     #.##### # #####.#          #.##         ##.#          #.## ###_### ##.#          #.   #     #   .#          #.## ####### ##.#          #.##         ##.#          #.## ####### ##.#     ######.## ####### ##.#######............#............##.####.#####.#.#####.####.##.####.#####.#.#####.####.##o..##...............##..o####.##.##.#######.##.##.######.##.##.#######.##.##.####......##....#....##......##.##########.#.##########.##.##########.#.##########.##.........................############################"};

int difficulty;
int is_ai;
int map_load; // if ==  1 true
int high_score = 0;
extern int score;
extern int game_won;
int window_width, window_height;
extern int image_width, image_height;


void draw_menu(SDL_Renderer* renderer);
void draw_play_mode(SDL_Renderer* renderer);
void draw_play_ai(SDL_Renderer* renderer);
void draw_select_map(SDL_Renderer* renderer);

llist* init_ghosts(int difficulty){
    // if difficulty is easy init 2 ghosts
    // if hard init 4
    // peacefull init 0 
    llist *ghosts = init_llist();
    if (difficulty == PEACEFULL){
        return ghosts;
    }
    // The coords (13, 13) should be the coords just under the door of the ghost
    // house
    else if (difficulty == EASY){
        Ghost* blinky = ghostInit("blinky", left, SCATTER, ROW, 0, 13, 13);
        Ghost* clyde = ghostInit("clyde", left, SCATTER, 0, COL, 13, 13);
        llist_append(ghosts, blinky);
        llist_append(ghosts, clyde);
        return ghosts;
    }
    else{
        Ghost* blinky = ghostInit("blinky", left, SCATTER, ROW, 0, 13, 13);
        Ghost* clyde = ghostInit("clyde", left, SCATTER, 0, COL, 13, 13);
        Ghost* inky = ghostInit("inky", right, SCATTER, 0, 0, 13, 13);
        Ghost* pinky = ghostInit("pinky", right, SCATTER, ROW, COL, 13, 13);
        llist_append(ghosts, blinky);
        llist_append(ghosts, clyde);
        llist_append(ghosts, inky);
        llist_append(ghosts, pinky);
        return ghosts;
    }
}

// get input from keyboard
int getch(){
    struct termios oldtc, newtc;
    int ch;
    tcgetattr(STDIN_FILENO, &oldtc);
    newtc = oldtc;
    newtc.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newtc);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldtc);
    return ch;
}

// case for arrow keys but not ready yet
int get_input(){
    int input = getch();
    if (input == 27){
        input = getch();
        if (input == 91){
            input = getch();
            return input;
        }
    }
    return input;
}



Game *init_game(int is_ai, int difficulty, int map_load){
    Game *game;
    game = malloc(sizeof(Game));
    game->difficulty = difficulty;
    game->globalGhostMode = SCATTER;
    game->globalGhostFrightened = 0;
    game->globalGhostKill = 0;
    if (map_load != 0){
        //game->map = load_map("./maps/map_1.txt");
        /*
        Map *map = malloc(sizeof(Map));
        map->grid = malloc(sizeof(char) * (ROW * COL));
        map->points = 0;
        for(size_t i = 0; i < ROW * COL; i++){
            map->grid[i] = DEFAULT_MAP[i];
        }
        game->map = map;*/
        game->map = init_map();

    }
    else{
        game->map = init_map();
    }
    game->pacman = init_player();
    game->ghosts = init_ghosts(difficulty);
    game->is_ai = is_ai;
	game->round = 0;
    return game;
}

void free_game(Game* game){
	llist_free(game->ghosts);
	free_map(game->map);
	free_player(game->pacman);
	free(game);	
}

int game_over(Game* game){
    if (game->pacman->lives == 0){
        return 1;
    }
    else{
        return 0;
    }
}

void updateGhostMode(Game* game){
    // update ghost mode
    // update global game mode
    int gr = game->round;
    int change = 0;
    if (gr == 0 || gr == 27 || gr == 54 || gr == 79){
        game->globalGhostMode = SCATTER;
        change = 1;
    }
    else if (gr == 7 || gr == 34 || gr == 59 || gr == 84){
        game->globalGhostMode = CHASE;
        change = 1;
    }
    
    // update ghost mode for each ghost if there is a change
    for (size_t i = 0; i < game->ghosts->length; i++){
        Ghost* ghost = llist_use(game->ghosts, i);
        switch(ghost->mode){
            case DEAD:
                // if the ghost is back at his spawn point, change his mode
                if (ghost->x == ghost->spawnX && ghost->y == ghost->spawnY){
                    ghost->mode = game->globalGhostMode;
                }
                break;
            case FRIGHTENED:
                // once the frightened time is over, change the ghost mode
                if (game->globalGhostFrightened == game->round){
                    ghost->mode = game->globalGhostMode;
                }
                break;
            case SCATTER:
                // if the global mode changes, change the ghost mode
                // or if the timer after a ghost has killed pacman is over
                if (change || game->globalGhostKill == game->round){
                    ghost->mode = game->globalGhostMode;
                }
                break;
            case CHASE:
                // if the global mode changes, change the ghost mode
                if (change){
                    ghost->mode = game->globalGhostMode;
                }
                break;
        }
        //printf("GhostName: %s -> mode : %i, 
        //targetTiles : %i,%i, direction : %i\n",ghost->name,
        //ghost->mode, ghost->targetX, ghost->targetY, ghost->direction);

    }
}

int update(Game* game){
    // check each entities direction and update x and y values accordingly
    // update ghosts
    for (size_t i = 0; i < game->ghosts->length; i++)
        GhostMove(llist_use(game->ghosts, i), llist_use(game->ghosts, 0),
                        game->map->grid, game->pacman);
    // update pacman
	int x = game->pacman->x;
	int y = game->pacman->y;
	int pac_location = y * COL + x;
    //check if the next position for pacman is valid
    if (game->map->grid[pac_location + game->pacman->direction] != WALL &&
        game->map->grid[pac_location + game->pacman->direction] != WALL2){
        pac_location += game->pacman->direction;
    }
	game->pacman->x = pac_location % COL;
	game->pacman->y = pac_location / COL;
	for (size_t i = 0; i < game->ghosts->length; i++){
		Ghost *ghost = llist_use(game->ghosts, i);
		int ghost_location = ghost->x + COL * ghost->y;
		// check if pacman and ghost overlap and do what is needed
		if (ghost_location == pac_location){
			if (ghost->mode == SCATTER || ghost->mode == CHASE){
				player_respawn(game->pacman);
				game->map->points += DEATH;
                // set the ghosts to scatter for 5 rounds after respawn
                game->globalGhostKill = game->round + 5;
                for (size_t j = 0; j < game->ghosts->length; j++){
                    Ghost* ghost = llist_use(game->ghosts, j);
                    ghost->mode = SCATTER;
                }
				if (game_over(game)){
					game->map->points += GAME_OVER;
				}
                return DEAD;
			}
			else{
				if (ghost->mode == FRIGHTENED){
					game->map->points += EAT_GHOST;
					ghost->mode = DEAD;
				}
			}
		}
	}
	// update score if needed and remove objects if needed
	if (game->map->grid[pac_location] != ' '){
		if (game->map->grid[pac_location] == '.'){
			game->map->points += TICTAC;
		}
		else{
			game->map->points += POWER_PELLET;
            // set all ghosts to frightened if not dead
            for(size_t i = 0; i < game->ghosts->length; i++){
                Ghost* ghost = llist_use(game->ghosts, i);
                if (ghost->mode != DEAD){
                    ghost->mode = FRIGHTENED;
                    // set the amount of rounds the ghosts will be frightened
                    game->globalGhostFrightened = game->round + 20;
                }
            }
            game->map->grid[pac_location] = ' ';
            updateGhostMode(game);
            game->round++;
            return POWER_PELLET;
		}
		game->map->grid[pac_location] = ' ';
	}
    // update the ghost mode
    updateGhostMode(game);
	game->round++;
    return 0;
}

int all_eaten(Game *game){
    int f = 0;
    int i = 0;
    while (i < ROW * COL && f != 1){
        char c = game->map->grid[i];
        if (c == '.' || c == 'o'){
            f = 1;
        }
        i++;
    }
    return f;
}



void draw_final(SDL_Renderer* renderer)
{
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer,11,63,114,255);

    if (game_won == 0)
    {
        SDL_Surface* sm_surface = IMG_Load("menuimg/game_over.png");
        SDL_Texture* sm_texture = SDL_CreateTextureFromSurface(renderer, sm_surface);
        SDL_FreeSurface(sm_surface);

        SDL_QueryTexture(sm_texture, NULL, NULL, &image_width, &image_height);
        SDL_QueryTexture(sm_texture, NULL, NULL, &image_width, &image_height);
        SDL_Rect sm_rect = { 0, 0, image_width, image_height};
        
        SDL_RenderCopy(renderer, sm_texture, NULL, &sm_rect);
        SDL_RenderPresent(renderer);
        sleep(2);
        SDL_DestroyTexture(sm_texture);
    }
    else
    {
        SDL_Surface* sm_surface = IMG_Load("menuimg/game_won.png");
        SDL_Texture* sm_texture = SDL_CreateTextureFromSurface(renderer, sm_surface);
        SDL_FreeSurface(sm_surface);

        SDL_QueryTexture(sm_texture, NULL, NULL, &image_width, &image_height);
        SDL_QueryTexture(sm_texture, NULL, NULL, &image_width, &image_height);
        SDL_Rect sm_rect = { 0, 0, image_width, image_height};
        
        SDL_RenderCopy(renderer, sm_texture, NULL, &sm_rect);
        SDL_RenderPresent(renderer);
        sleep(2);
        SDL_DestroyTexture(sm_texture);

    }

    SDL_Surface* sm_surface = IMG_Load("menuimg/final.png");
    SDL_Texture* sm_texture = SDL_CreateTextureFromSurface(renderer, sm_surface);
    SDL_FreeSurface(sm_surface);

    SDL_QueryTexture(sm_texture, NULL, NULL, &image_width, &image_height);
    SDL_QueryTexture(sm_texture, NULL, NULL, &image_width, &image_height);
    SDL_Rect sm_rect = { 0, 0, image_width, image_height};
    
    SDL_RenderCopy(renderer, sm_texture, NULL, &sm_rect);

    TTF_Font* font = TTF_OpenFont("Paperkind.ttf", 100); 
    SDL_Color text_color = { 255, 255, 255, 255 }; 

    char score_text[15]; // Assuming the high score will be within 10 digits
    sprintf(score_text, "%d", score); /// needs to change it to score =)

    int text_width, text_height;
    TTF_SizeText(font, score_text, &text_width, &text_height);

    if (score>high_score)
    {
        high_score = score;
    }

    int screen_width, screen_height;
    SDL_GetRendererOutputSize(renderer, &screen_width, &screen_height);

    int text_x = (screen_width - text_width) / 2;
    int text_y = (screen_height - text_height) / 2;

    SDL_Surface* text_surface = TTF_RenderText_Solid(font, score_text, text_color);
    SDL_Texture* text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
    SDL_FreeSurface(text_surface);

    SDL_Rect text_rect = { text_x, text_y, text_width, text_height };

    SDL_RenderCopy(renderer, text_texture, NULL, &text_rect);
    SDL_DestroyTexture(text_texture);
    TTF_CloseFont(font);

    SDL_RenderPresent(renderer);

    SDL_Event event;
    while (SDL_WaitEvent(&event)) {
    
        if (event.type == SDL_QUIT) {
            break;
        } 
        else if (event.type == SDL_MOUSEBUTTONDOWN) {
            int x = event.button.x;
            int y = event.button.y;
            if (x >= 86 && x <= 358 &&
                y >= 726 && y <= 798) {
                draw_menu(renderer); 
            }
            else if (x >= 479 && x <= 721 &&
             y >= 726 && y <= 798)
            {
                SDL_SetRenderDrawColor(renderer,0,0,0,255);
                Game* game = init_game(is_ai, difficulty,map_load);
                draw_game(renderer,game,map_load);
            }
            else if (x>= 887 && x<= 1031&& y>= 726 &&y<=798)
            {
                SDL_DestroyTexture(sm_texture);
                SDL_DestroyTexture(text_texture);
                SDL_DestroyRenderer(renderer);
                TTF_Quit();
                IMG_Quit();
                SDL_Quit();
                exit(0);
            }
            
        }
    }


}

void draw_map(SDL_Renderer* renderer)
{
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer,11,63,114,255);
    SDL_Surface* play_surface = IMG_Load("menuimg/map.png");
    SDL_Texture* play_texture = SDL_CreateTextureFromSurface(renderer, play_surface);
    SDL_FreeSurface(play_surface);

    SDL_QueryTexture(play_texture, NULL, NULL, &image_width, &image_height);
    SDL_Rect play_rect = { 0, 0, image_width, image_height};
    
    SDL_RenderCopy(renderer, play_texture, NULL, &play_rect);

    int buttonwidth = 300;
    int buttonheight = 300;

    int back_x = 450;
    int back_y = 713;
    int ai_y = (window_height - buttonheight) / 2;
    int me_x = 700;
    int ai_x = 200;

    SDL_RenderPresent(renderer);

    SDL_Event event;
    while (SDL_WaitEvent(&event)) {
        
        if (event.type == SDL_QUIT) {
            SDL_DestroyTexture(play_texture);
            SDL_DestroyRenderer(renderer);
            TTF_Quit();
            IMG_Quit();
            SDL_Quit();
            exit(0);
            
        } 
        else if (event.type == SDL_MOUSEBUTTONDOWN) {
            int x = event.button.x;
            int y = event.button.y;
            if (x >= back_x && x <= back_x + 300 &&
                y >= back_y && y <= back_y + 60) {
                draw_play_ai(renderer);
            }
            else if (x >= ai_x && x <= ai_x + buttonwidth &&
                y >= ai_y && y <= ai_y + buttonheight) {
                map_load = 1;
                draw_select_map(renderer);
            }
            else if (x >= me_x && x <= me_x + buttonwidth &&
             y >= ai_y && y <= ai_y + buttonheight) {
                SDL_SetRenderDrawColor(renderer,0,0,0,255);
                map_load = 0;
                Game* game = init_game(is_ai, difficulty,map_load);
                draw_game(renderer,game,map_load);
            }
        }
    }
    SDL_DestroyTexture(play_texture);
}

void draw_play_ai(SDL_Renderer* renderer)
{
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer,11,63,114,255);
    SDL_Surface* play_surface = IMG_Load("menuimg/who.png");
    SDL_Texture* play_texture = SDL_CreateTextureFromSurface(renderer, play_surface);
    SDL_FreeSurface(play_surface);

    SDL_QueryTexture(play_texture, NULL, NULL, &image_width, &image_height);
    SDL_Rect play_rect = { 0, 0, image_width, image_height};
    
    SDL_RenderCopy(renderer, play_texture, NULL, &play_rect);

    int buttonwidth = 300;
    int buttonheight = 300;

    int back_x = 450;
    int back_y = 713;
    int ai_y = (window_height - buttonheight) / 2;
    int me_x = 700;
    int ai_x = 200;

    SDL_RenderPresent(renderer);

    SDL_Event event;
    while (SDL_WaitEvent(&event)) {
        
        if (event.type == SDL_QUIT) {
            SDL_DestroyTexture(play_texture);
            SDL_DestroyRenderer(renderer);
            TTF_Quit();
            IMG_Quit();
            SDL_Quit();
            exit(0);
            
        } 
        else if (event.type == SDL_MOUSEBUTTONDOWN) {
            int x = event.button.x;
            int y = event.button.y;
            if (x >= back_x && x <= back_x + 300 &&
                y >= back_y && y <= back_y + 60) {
                draw_play_mode(renderer);
            }
            else if (x >= ai_x && x <= ai_x + buttonwidth &&
                y >= ai_y && y <= ai_y + buttonheight) {
                is_ai = 1;
                draw_map(renderer);
            }
            else if (x >= me_x && x <= me_x + buttonwidth &&
             y >= ai_y && y <= ai_y + buttonheight) {
                is_ai = 0;
                draw_map(renderer);
            }
        }
    }
    SDL_DestroyTexture(play_texture);
}

void draw_play_mode(SDL_Renderer* renderer)
{
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer,11,63,114,255);
    SDL_Surface* play_surface = IMG_Load("menuimg/play.png");
    SDL_Texture* play_texture = SDL_CreateTextureFromSurface(renderer, play_surface);
    SDL_FreeSurface(play_surface);

    SDL_QueryTexture(play_texture, NULL, NULL, &image_width, &image_height);
    SDL_Rect play_rect = { 0, 0, image_width, image_height};
    
    SDL_RenderCopy(renderer, play_texture, NULL, &play_rect);

    SDL_RenderPresent(renderer);

    int buttonwidth = 500;
    int buttonheight = 100;
    int buttonmargin = 60;

    int sum = buttonheight+buttonmargin;
    int easy_x = 350;
    int easy_y = 244;
    int medium_y = easy_y + sum;
    int hard_y = medium_y + sum;
    int back_x = 450;
    int back_y = 713;

    SDL_Event event;
    while (SDL_WaitEvent(&event)) {
        
        if (event.type == SDL_QUIT) {
            SDL_DestroyTexture(play_texture);
            SDL_DestroyRenderer(renderer);
            TTF_Quit();
            IMG_Quit();
            SDL_Quit();
            exit(0);
            
        } 
        else if (event.type == SDL_MOUSEBUTTONDOWN) {
            int x = event.button.x;
            int y = event.button.y;
            if (x >= back_x && x <= back_x + 300 &&
                y >= back_y && y <= back_y + 60) {
                draw_menu(renderer);
            }
            else if (x >= easy_x && x <= easy_x + buttonwidth &&
                y >= easy_y && y <= easy_y + buttonheight) {
                difficulty = PEACEFULL;
                draw_play_ai(renderer);
            }
            else if (x >= easy_x && x <= easy_x + buttonwidth &&
             y >= medium_y && y <= medium_y + buttonheight) {
                difficulty = EASY;
                draw_play_ai(renderer);
            }
            else if (x >= easy_x && x <= easy_x + buttonwidth &&
             y >= hard_y && y <= hard_y + buttonheight) {
                difficulty = HARD;
                draw_play_ai(renderer);
            }
        }
    }
    SDL_DestroyTexture(play_texture);


}

void draw_select_map(SDL_Renderer* renderer)
{
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 11, 63, 114, 255);
    SDL_RenderPresent(renderer);

    SDL_Surface* sm_surface = IMG_Load("menuimg/button.png");
    SDL_Texture* sm_texture = SDL_CreateTextureFromSurface(renderer, sm_surface);
    SDL_FreeSurface(sm_surface);

    SDL_QueryTexture(sm_texture, NULL, NULL, &image_width, &image_height);
    SDL_QueryTexture(sm_texture, NULL, NULL, &image_width, &image_height);
    SDL_Rect sm_rect = { 0, 0, image_width, image_height};
    
    SDL_RenderCopy(renderer, sm_texture, NULL, &sm_rect);
    int back_x = 450;
    int back_y = 713; 

    SDL_RenderPresent(renderer);
    SDL_Event event;
    while (SDL_WaitEvent(&event)) {
    
        if (event.type == SDL_QUIT) {
            SDL_DestroyTexture(sm_texture);
            SDL_DestroyRenderer(renderer);
            TTF_Quit();
            IMG_Quit();
            SDL_Quit();
            exit(0);
            
        } 
        else if (event.type == SDL_MOUSEBUTTONDOWN) {
            int x = event.button.x;
            int y = event.button.y;
            if (x >= back_x && x <= back_x + 300 &&
                y >= back_y && y <= back_y + 60) {
                draw_map(renderer); 
            }
            else if (x >= 0 && x <= 1200 &&
             y >= 0 && y <= 928)
            {
                SDL_SetRenderDrawColor(renderer,0,0,0,255);
                Game* game = init_game(is_ai, difficulty,map_load);
                draw_game(renderer,game,map_load);
            }
        }
    }
    SDL_DestroyTexture(sm_texture);
   
}


void draw_select_map_menu(SDL_Renderer* renderer)
{
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 11, 63, 114, 255);
    SDL_RenderPresent(renderer);

    SDL_Surface* sm_surface = IMG_Load("menuimg/button.png");
    SDL_Texture* sm_texture = SDL_CreateTextureFromSurface(renderer, sm_surface);
    SDL_FreeSurface(sm_surface);

    SDL_QueryTexture(sm_texture, NULL, NULL, &image_width, &image_height);
    SDL_QueryTexture(sm_texture, NULL, NULL, &image_width, &image_height);
    SDL_Rect sm_rect = { 0, 0, image_width, image_height};
    
    SDL_RenderCopy(renderer, sm_texture, NULL, &sm_rect);
    int back_x = 450;
    int back_y = 713; 


    SDL_RenderPresent(renderer);
    SDL_Event event;
    while (SDL_WaitEvent(&event)) {
    
        if (event.type == SDL_QUIT) {
            SDL_DestroyTexture(sm_texture);
            SDL_DestroyRenderer(renderer);
            TTF_Quit();
            IMG_Quit();
            SDL_Quit();
            exit(0);
        } 
        else if (event.type == SDL_MOUSEBUTTONDOWN) {
            int x = event.button.x;
            int y = event.button.y;
            if (x >= back_x && x <= back_x + 300 &&
                y >= back_y && y <= back_y + 135) {
                draw_menu(renderer); 
            }
            else if (x >= 0 && x <= 1200 &&
             y >= 0 && y <= 928)
            {
                //todo : function to put map in an array
            }
        }
    }
    SDL_DestroyTexture(sm_texture);
   
}

void draw_high_score(SDL_Renderer* renderer)
{
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 11, 63, 114, 255);
    SDL_RenderPresent(renderer);

    SDL_Surface* sm_surface = IMG_Load("menuimg/high_score.png");
    SDL_Texture* sm_texture = SDL_CreateTextureFromSurface(renderer, sm_surface);
    SDL_FreeSurface(sm_surface);

    SDL_QueryTexture(sm_texture, NULL, NULL, &image_width, &image_height);
    SDL_QueryTexture(sm_texture, NULL, NULL, &image_width, &image_height);
    SDL_Rect sm_rect = { 0, 0, image_width, image_height};
    
    SDL_RenderCopy(renderer, sm_texture, NULL, &sm_rect);
    int back_x = 450;
    int back_y = 713; 

    TTF_Font* font = TTF_OpenFont("Paperkind.ttf", 100); 
    SDL_Color text_color = { 255, 255, 255, 255 }; 

    char score_text[15]; // Assuming the high score will be within 10 digits
    sprintf(score_text, "%d", high_score);

    int text_width, text_height;
    TTF_SizeText(font, score_text, &text_width, &text_height);

    int screen_width, screen_height;
    SDL_GetRendererOutputSize(renderer, &screen_width, &screen_height);

    int text_x = (screen_width - text_width) / 2;
    int text_y = (screen_height - text_height) / 2;

    SDL_Surface* text_surface = TTF_RenderText_Solid(font, score_text, text_color);
    SDL_Texture* text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
    SDL_FreeSurface(text_surface);

    SDL_Rect text_rect = { text_x, text_y, text_width, text_height };

    SDL_RenderCopy(renderer, text_texture, NULL, &text_rect);
    SDL_DestroyTexture(text_texture);
    TTF_CloseFont(font);

    SDL_RenderPresent(renderer);
    SDL_Event event;
    while (SDL_WaitEvent(&event)) {
    
        if (event.type == SDL_QUIT) {
            SDL_DestroyTexture(sm_texture);
            SDL_DestroyTexture(text_texture);
            SDL_DestroyRenderer(renderer);
            TTF_Quit();
            IMG_Quit();
            SDL_Quit();
            exit(0);
        } 
        else if (event.type == SDL_MOUSEBUTTONDOWN) {
            int x = event.button.x;
            int y = event.button.y;
            if (x >= back_x && x <= back_x + 300 &&
                y >= back_y && y <= back_y + 135) {
                draw_menu(renderer); 
            }
        }
    }
    SDL_DestroyTexture(sm_texture);
   
}

void draw_help(SDL_Renderer* renderer)
{
    SDL_RenderClear(renderer);

    SDL_Surface* help_surface = IMG_Load("menuimg/help.png");
    SDL_Texture* help_texture = SDL_CreateTextureFromSurface(renderer, help_surface);
    SDL_FreeSurface(help_surface);

    SDL_QueryTexture(help_texture, NULL, NULL, &image_width, &image_height);
    SDL_Rect help_rect = { 0, 0, image_width, image_height};
    
    SDL_RenderCopy(renderer, help_texture, NULL, &help_rect);

    SDL_RenderPresent(renderer);
    int back_x = 450;
    int back_y = 713;

    SDL_Event event;
    while (SDL_WaitEvent(&event)) {
        
        if (event.type == SDL_QUIT) {
            SDL_DestroyTexture(help_texture);
            SDL_DestroyRenderer(renderer);
            TTF_Quit();
            IMG_Quit();
            SDL_Quit();
            exit(0);
        } 
        else if (event.type == SDL_MOUSEBUTTONDOWN) {
            int x = event.button.x;
            int y = event.button.y;
            if (x >= back_x && x <= back_x + 300 &&
                y >= back_y && y <= back_y + 60) {
                draw_menu(renderer);
            }
        }
    }
    SDL_DestroyTexture(help_texture);
}

void draw_about(SDL_Renderer* renderer)
{
    SDL_RenderClear(renderer);

    SDL_Surface* about_surface = IMG_Load("menuimg/about.png");
    SDL_Texture* about_texture = SDL_CreateTextureFromSurface(renderer, about_surface);
    SDL_FreeSurface(about_surface);

    SDL_QueryTexture(about_texture, NULL, NULL, &image_width, &image_height);
    SDL_Rect about_rect = { 0, 0, image_width, image_height};
    
    SDL_RenderCopy(renderer, about_texture, NULL, &about_rect);

    SDL_RenderPresent(renderer);
    int back_x = 450;
    int back_y = 713;

    SDL_Event event;
    while (SDL_WaitEvent(&event)) {
        
        if (event.type == SDL_QUIT) {
            SDL_DestroyTexture(about_texture);
            SDL_DestroyRenderer(renderer);
            TTF_Quit();
            IMG_Quit();
            SDL_Quit();
            exit(0);
        } 
        else if (event.type == SDL_MOUSEBUTTONDOWN) {
            int x = event.button.x;
            int y = event.button.y;
            if (x >= back_x && x <= back_x + 300 &&
                y >= back_y && y <= back_y + 60) {
                draw_menu(renderer);
            }
        }
    }
    SDL_DestroyTexture(about_texture);
}

void draw_menu(SDL_Renderer* renderer)
{
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer,11,63,114,255);
    SDL_Surface* menu_surface = IMG_Load("menuimg/menu.png");
    SDL_Texture* menu_texture = SDL_CreateTextureFromSurface(renderer, menu_surface);
    SDL_FreeSurface(menu_surface);

    SDL_QueryTexture(menu_texture, NULL, NULL, &image_width, &image_height);
    SDL_Rect menu_rect = { 0, 0, image_width, image_height};
    
    SDL_RenderCopy(renderer, menu_texture, NULL, &menu_rect);

    SDL_RenderPresent(renderer);

    int buttonwidth = 300;
    int buttonheight = 60;
    int buttonmargin = 35;
    int sum = buttonheight+buttonmargin;
    int play_x = 840;
    int play_y = 142;
    int select_map_y = play_y + sum;
    int high_score_y = select_map_y + sum;
    int help_y = high_score_y + sum;
    int about_y = help_y + sum;
    int quit_y = about_y + sum;

    SDL_Event event;
    while (SDL_WaitEvent(&event)) {
        
        if (event.type == SDL_QUIT) {
            break;
        } 
        else if (event.type == SDL_MOUSEBUTTONDOWN) {
            int x = event.button.x;
            int y = event.button.y;
            if (x >= play_x && x <= play_x + buttonwidth &&
                y >= play_y && y <= play_y + buttonheight) {
                // Handle Play button click
                draw_play_mode(renderer);
            }
            else if (x >= play_x && x <= play_x + buttonwidth &&
             y >= select_map_y && y <= select_map_y + buttonheight) {
                // Handle Select Map button click
                draw_select_map_menu(renderer);
            }
            else if (x >= play_x && x <= play_x + buttonwidth &&
             y >= high_score_y && y <= high_score_y + buttonheight) {
                // Handle High Score button click
                draw_high_score(renderer);
            }
            else if (x >= play_x && x <= play_x + buttonwidth &&
             y >= help_y && y <= help_y + buttonheight) {
                // Handle Help button click
                draw_help(renderer);
            }
            else if (x >= play_x && x <= play_x + buttonwidth &&
             y >= about_y && y <= about_y + buttonheight) {
                draw_about(renderer);
            }
            else if (x >= play_x && x <= play_x + buttonwidth &&
             y >= quit_y && y <= quit_y + buttonheight) {
                SDL_DestroyTexture(menu_texture);
                SDL_DestroyRenderer(renderer);
                TTF_Quit();
                IMG_Quit();
                SDL_Quit();
                exit(0);
            } 
        }
    }
    SDL_DestroyTexture(menu_texture);
}

int main() {
    // Initialize SDL
    SDL_Init(SDL_INIT_VIDEO);

    // Initialize SDL_Image
    IMG_Init(IMG_INIT_PNG);
    TTF_Init();

    // Create a window
    SDL_Window* window = SDL_CreateWindow(
        "Pac-Cracked",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        1200, 928,
        SDL_WINDOW_SHOWN  
    );

    SDL_GetWindowSize(window, &window_width, &window_height);

    // Create a renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(
        window, -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );
    SDL_SetRenderDrawColor(renderer,11,63,114,255);

    draw_menu(renderer);

    // Wait for the user to quit
    SDL_Event event;

    while (SDL_WaitEvent(&event))
    {
        if (event.type == SDL_QUIT) {
            break;
        }
    }
    // Clean up
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    TTF_Quit();
    SDL_Quit();

    return 0;
}