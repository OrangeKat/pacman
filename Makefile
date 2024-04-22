CC = gcc
CFLAGS = -Wall -Wextra -Werror  -pedantic -std=c99 -g -fsanitize=address -lm `pkg-config --cflags sdl2 SDL2_image SDL2_ttf`
LDFLAGS =
LDLIBS = `pkg-config --libs sdl2 SDL2_image SDL2_ttf`

# Add all the .c files from obj, src, pacman_ai, and pacman_ai/lib to the list of files to compile
SRC = $(wildcard src/*.c) $(wildcard obj/*.c) $(wildcard pacman_ai/*.c) $(wildcard pacman_ai/lib/*.c) mainmenu.c play.c
OBJ = $(SRC:.c=.o)

all: game

game: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) $(LDLIBS)

.PHONY: clean

clean:
	rm -f $(OBJ) game
