CC=cc
SDL_LIB = -L/opt/local/lib -lSDL2 -lSDL2_ttf -lSDL2_mixer -lgc
SDL_INCLUDE = -I/opt/local/include/
LDFLAGS = $(SDL_LIB)

all: single-letter 

single-letter: main.o
	$(CC) $< $(LDFLAGS) -o $@

main.o: main.c
	$(CC) -c $(SDL_INCLUDE) $< 

clean:
	rm *.o && rm single-letter 

