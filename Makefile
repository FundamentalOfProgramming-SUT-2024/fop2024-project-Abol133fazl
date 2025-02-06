CC = gcc
CFLAGS = -Wall -std=c99 -g
LDFLAGS = -lncursesw -lSDL2 -lSDL2_mixer
SOURCES = main.c menus.c makemap.c enemies.c themes.c updates.c saving.c music.c draws.c move.c specialmaps.c asciarts.c
HEADERS = menus.h makemap.h enemies.h
OBJECTS = $(SOURCES:.c=.o)
EXEC = game

all: $(EXEC)

$(EXEC): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(EXEC) $(OBJECTS) $(LDFLAGS)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(EXEC)

run: $(EXEC)
	./$(EXEC)
