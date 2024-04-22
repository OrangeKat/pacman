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
#include <unistd.h>
#include "pacman_ai/search.h"
#include <time.h>

char DEFAULT_MAP[] = {"############################............#............##.####.#####.#.#####.####.##o####.#####.#.#####.####o##.####.#####.#.#####.####.##.........................##.####.##.#######.##.####.##.####.##.#######.##.####.##......##....#....##......#######.##### # #####.######     #.##### # #####.#          #.##         ##.#          #.## ###_### ##.#          #.   #     #   .#          #.## ####### ##.#          #.##         ##.#          #.## ####### ##.#     ######.## ####### ##.#######............#............##.####.#####.#.#####.####.##.####.#####.#.#####.####.##o..##...............##..o####.##.##.#######.##.##.######.##.##.#######.##.##.####......##....#....##......##.##########.#.##########.##.##########.#.##########.##.........................############################"};

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

void randomMapPath(char* pathPng, char* pathTxt){
    // pathPng and pathTxt should be heap allocated
    srand(time(NULL));
    int r = rand() % 6 + 1;
    // path -> ./png/map_x.png
    // with x a value between 1 and 6
    sprintf(pathPng, "./png/map_%d.png", r);
    // path -> ./maps/map_x.txt
    // with x a value between 1 and 6
    sprintf(pathTxt, "./maps/map_%d.txt", r);
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
        Map *map = malloc(sizeof(Map));
        map->grid = malloc(sizeof(char) * (ROW * COL));
        map->points = 0;
        for(size_t i = 0; i < ROW * COL; i++){
            map->grid[i] = DEFAULT_MAP[i];
        }
        game->map = map;
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

int main(int argc, char** argv){
    if (argc != 3){
        printf("Usage: ./game [difficulty] [is_ai]\n");
        exit(EXIT_FAILURE);
    }
    // - Launch the SDL Interface
    // - On Play Button Click
    // - Select Difficulty and if want to load map
    // - Select pacman is played by ai or human
    // - Load gameplay loop:
    //   .Generate / Load map
    //   .Init player and ghosts
	int is_ai = argv[2][0] - '0';
	int difficulty = argv[1][0] - '0';
	int map_load = 0;
    int ch = 0;
    int prev_ch = 0;
    int countdown = 10;
    printf("Initiating game...\n");
	Game* game = init_game(is_ai, difficulty, map_load);
	//gtree* minimax_tree = NULL;
    printf("Initiating extra assets...\n");
	//if (game->is_ai == 1 && difficulty > 2){
	//	minimax_tree = create_tree(game, 6);
	//}
    int state = 0;
    printf("Starting game...\n");
	while (game_over(game) == 0 && all_eaten(game) == 1){
    //   .Start Gameplay loop (while not_won):
    //      _Print Board
        printf("Score: %d\n", game->map->points);
        printf("Lives: %d\n", game->pacman->lives);
        print_map(game->map, game->pacman, game->ghosts);
		fflush(stdout);
    //      _Update Ghost Direction
    //      _Update Pacman Direction
    //      _Update location
    //      _Update Score
        printf("Updating game state...\n");
		if (game->is_ai != 1){
            ch = get_input();
            if (ch == 0){
                ch = prev_ch;
                //printf("No input detected, using previous input\n");
            }
            else{
                prev_ch = ch;
                //printf("Input detected, using new input\n");
            }
            int pos = game->pacman->x + game->pacman->y * COL;
            switch (ch) {
                case KEY_UP:
                    if (game->map->grid[pos + UP] != WALL && game->map->grid[pos + UP] != WALL2){
                        game->pacman->direction = UP;
                    }
                    break;
                case KEY_DOWN:
                    if (game->map->grid[pos + DOWN] != WALL && game->map->grid[pos + DOWN] != WALL2){
                        game->pacman->direction = DOWN;
                    }
                    break;
                case KEY_LEFT:
                    if (game->map->grid[pos + LEFT] != WALL && game->map->grid[pos + LEFT] != WALL2){
                        game->pacman->direction = LEFT;
                    }
                    break;
                case KEY_RIGHT:
                    if (game->map->grid[pos + RIGHT] != WALL && game->map->grid[pos + RIGHT] != WALL2){
                        game->pacman->direction = RIGHT;
                    }
                    break;
                default:
                    break;
            }
            
			state = update(game);
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
            
            state = update(game);
		}
        /*
        if (countdown == 0){
            for (int i = 0; i < game->difficulty; i++){
                Ghost *g = llist_use(game->ghosts, i);
                g->mode = CHASE;
            }
        }
        else{
            countdown--;
        }

        GhostMode tmp = CHASE;
        if (state != 0){
            if (state == POWER_PELLET){
                tmp = FRIGHTENED;
                countdown += 10;
            }
            if (state == DEAD){
                tmp = SCATTER;
            }
            for (int i = 0; i < game->difficulty; i++){
                Ghost *g = llist_use(game->ghosts, i);
                g->mode = tmp;
                countdown += 10;
            }
            state = 0;
        }
        */
        state = countdown;
        countdown = state;
        if (game->pacman->lives == 0){
            break;
        }
		// Makes loop sleep for 1s
		//sleep(1);
	}
    if (game->pacman->lives == 0){
        printf("Game over...\n");
    }
    else{
        printf("YOU WIN!\n");
    }
    printf("Score: %d\n", game->map->points);
    //if (minimax_tree != NULL){
	//    free_minimax(minimax_tree);
    //}
    // - When game done ask if player wants to save map
	// - Ask for save file name TODO
	char name[128];
	int save = 0;
	if (save == 1){
		save_map(game->map, name);
	}
    // - Free all game assets
	free_game(game);
    // - End game screen with score, replay button, close game button
    // - If close:
    //   .Close SDL window
    //   .Destroy all elements
	return 1;
}
