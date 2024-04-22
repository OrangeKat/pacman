CC=gcc
CFLAGS=-Wall -Wextra -Werror -pedantic -std=c99 -g `pkg-config --cflags sdl2 SDL2_image`
LDLIBS=`pkg-config --libs sdl2 SDL2_image`

SRCT = test.c
OBJT = $(SRCT:.c=.o)

SRC = main.c txtToPNG.c
OBJ = $(SRC:.c=.o)

all: test main

test: $(OBJT)
	$(CC) $(CFLAGS) $(OBJT) -o test $(LDLIBS)

main: $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o main $(LDLIBS)

.PHONY: clean

clean:
	rm -f $(OBJ) main
	rm -f $(OBJT) test combined.png
