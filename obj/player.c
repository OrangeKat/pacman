#include <stdlib.h>
#include "player.h"

Player* init_player(){
	Player *pacman;
	pacman = malloc(sizeof(Player));
    pacman->x = SPAWN_X;
    pacman->y = SPAWN_Y;
    pacman->lives = 3;
    pacman->direction = IDLE;
	return pacman;
}

void free_player(Player *pacman){
	free(pacman);
}

void player_respawn(Player* pacman){
	pacman->lives -= 1;
	pacman->x = SPAWN_X;
	pacman->y = SPAWN_Y;
	pacman->direction = IDLE;
}
