#include <stdlib.h>
#include "lib/linked_list.h"
#include "minimax.h"
#include "lib/tree.h"
#include "../main.h"
#include <string.h>

int *can_goto(Game *game){
	int x = game->pacman->x;
	int y = game->pacman->y;
	int pos = y * COL + x;
	int* options = calloc(4, sizeof(int));
	char* grid = game->map->grid;
	if(grid[pos + COL] != WALL && grid[pos + COL] != WALL2){
		options[0] = COL;
	}
	if(grid[pos - COL] != WALL && grid[pos - COL] != WALL2){
		options[1] = -COL;
	}
	if(grid[pos + 1] != WALL && grid[pos + 1] != WALL2){
		options[2] = 1;
	}
	if(grid[pos - 1] != WALL && grid[pos + 1] != WALL2){
		options[3] = -1;
	}
	return options;
}

gtree *create_tree(Game *game, int depth){
	gtree* decision_tree = malloc(sizeof(gtree));
	llist* q = malloc(sizeof(llist));
	q->length = 0;
	decision_tree->key = game;
	int start = game->round;
	llist_prepend(q, decision_tree);
	while (q->length != 0){
		gtree* branch = llist_fastpop(q);
		Game *cur_state = branch->key;
		int *options = can_goto(cur_state);
		if ((cur_state->round != start + depth - 1) && (game_over(cur_state) != 1)){
			for (int i = 0; i < 4; i++){
				if (options[i] != 0){
					Game *new = malloc(sizeof(Game));
					memcpy(new, cur_state, sizeof(Game));
					new->pacman->direction = options[i];
					llist_append(branch->children, new);
					update(new);
					
				}
			}
		}
	}
	return decision_tree;
}

void update_tree(Game *game, gtree *tree){
	if (tree->children->length == 0){
		Game *cur_state = tree->key;
		int *options = can_goto(cur_state);
		if (game_over(cur_state) == 1){
			return;
		}
		for (int i = 0; i < 4; i++){
			if (options[i] != 0){
				Game *new = malloc(sizeof(Game));
				memcpy(new, cur_state, sizeof(Game));
				new->pacman->direction = options[i];
				llist_append(tree->children, new);
				update(new);
			}
		}
	}
	else{
		for (size_t i = 0; i < tree->children->length; i++){
			update_tree(game, llist_use(tree->children, i));
		}
	}
}

int __make_decision(gtree *tree){
	if (tree->children->length == 0){
		Game *game = tree->key;
		return game->map->points;
	}
	else{
		int max = 0;
		for (size_t i = 0; i < tree->children->length; i++){
				int tmp = __make_decision(llist_use(tree->children, i));
				if (max < tmp){
					max = tmp;
				}
			}
		return max;
	}
}

gtree *make_decision(gtree *tree){
	int max_i = 0;
	int max = 0;
	for (size_t i = 0; i < tree->children->length; i++){
		int tmp = __make_decision(llist_use(tree->children, i));
		if (max < tmp){
			max = tmp;
			max_i = i;
		}
	}
	return llist_use(tree->children, max_i);
}

void free_minimax(gtree* tree){
	for (size_t i = 0; i < tree->children->length; i++){
		free_minimax(llist_use(tree->children, i));
	}
	Game *game = tree->key;
	free_game(game);
	free(tree->key);
	free(tree->children);
	free(tree);
}

gtree *minimax(gtree* tree){
	gtree* to_keep = make_decision(tree);
	for (size_t i = 0; i < tree->children->length; i++){
		gtree* t = llist_use(tree->children, i);
		if (t != to_keep){
			free_minimax(t);
		}
	}
	Game *game = tree->key;
	free_game(game);
	free(tree->key);
	free(tree->children);
	free(tree);
	return to_keep;
}

