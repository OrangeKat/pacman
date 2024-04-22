CC=gcc
CFLAGS=-Wall -Wextra -Werror -pedantic -std=c99 -g -fsanitize=address -lm

# Add all the .C files from obj, src and game.c to the list of files to compile
SRC=$(wildcard src/*.c) $(wildcard obj/*.c) $(wildcard pacman_ai/*.c) $(wildcard pacman_ai/lib/*.c)  main.c
OBJ=$(SRC:.c=.o)

all: game

game: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

.PHONY: clean

clean:
	rm -f $(OBJ) game

