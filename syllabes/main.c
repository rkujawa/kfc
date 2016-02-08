#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <sys/syslimits.h>

#define SCREEN_WIDTH 1680
#define SCREEN_HEIGHT 1050

bool kfcTextToTex(const char *, SDL_Texture **, SDL_Color, int);
void kfcRenderSyllabe(const char *);
void kfcRenderTex(SDL_Texture *, int, int);
void logErrorSDL(const char *);

static const char *fontname = "/Library/Fonts/Arial Black.ttf";

static SDL_Color white = { 255, 255, 255 };
static SDL_Window *win;
static SDL_Renderer *ren;

#define SYLLABLES_COUNT 7
static const char *syllabes[] = { "PA", "PE", "PO", "PI", "PU", "PÓ", "PY" };

int main (int argc, char *argv[]) 
{
	char c; int i;
	i = 0;

	if (SDL_Init(SDL_INIT_EVERYTHING) != 0){
		logErrorSDL("SDL_Init Error");
		return 1;
	}

	win = SDL_CreateWindow("KFC!", 100, 100, SCREEN_WIDTH, 
	    SCREEN_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN);	
	if (win == NULL) {
		logErrorSDL("SDL_CreateWindow Error");
		SDL_Quit();
		return 1;
	}

	ren = SDL_CreateRenderer(win, -1, 
	    SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (ren == NULL) {
		SDL_DestroyWindow(win);
		SDL_Quit();
		return 1;
	}

		 	
	if (TTF_Init() == -1) {
		printf("TTF_Init: %s\n", TTF_GetError());
		return 1;
	}

	SDL_ShowCursor(SDL_DISABLE);

	SDL_RenderClear(ren);
	kfcRenderSyllabe(syllabes[i]);
	i++;
	SDL_RenderPresent(ren);

	while (1) {
		SDL_Event e;


		if (SDL_PollEvent(&e)) {

			if (e.type == SDL_QUIT)
				break;

			if (e.type == SDL_KEYDOWN) { 
				if (e.key.keysym.sym == SDLK_ESCAPE)
					break;

				printf("%d\n", e.key.keysym.sym) ;
				if (e.key.keysym.sym == SDLK_SPACE) {
					if (i == SYLLABLES_COUNT)
						break;
					SDL_RenderClear(ren);
					kfcRenderSyllabe(syllabes[i]);
					i++;
					SDL_RenderPresent(ren);
/*					SDL_Delay(3000);*/

				}

			}
		}
	}

	TTF_Quit();	
	SDL_DestroyRenderer(ren);
	SDL_DestroyWindow(win);
	SDL_Quit();

	return 0;
}

void kfcRenderSyllabe(const char *charBuf)
{
	static SDL_Texture *txttex;
	int iW, iH, x, y;

	if (!kfcTextToTex(charBuf, &txttex, white, 300))
		printf("kfcTextToTex failed\n");

	SDL_QueryTexture(txttex, NULL, NULL, &iW, &iH);
	x = SCREEN_WIDTH / 2 - iW / 2;
	y = SCREEN_HEIGHT / 2 - iH / 2;
	kfcRenderTex(txttex, x, y);
}

void logErrorSDL(const char *msg) 
{
	fprintf(stderr, "%s: %s\n", msg, SDL_GetError());
}

bool kfcTextToTex(const char *msg, SDL_Texture **fonttex, SDL_Color color, 
    int fontsize)
{
	SDL_Surface *surf;
	TTF_Font *font;

	font = TTF_OpenFont(fontname, fontsize);
	if (font == NULL) {
		logErrorSDL("TTF_OpenFont");
		return false;
	}	

	surf = TTF_RenderUTF8_Solid(font, msg, color);
	if (surf == NULL) {
		TTF_CloseFont(font);
		logErrorSDL("TTF_RenderText");
		return false;
	}

	*fonttex = SDL_CreateTextureFromSurface(ren, surf);
	if (fonttex == NULL) {
		logErrorSDL("CreateTexture");
		return false;
	}


	TTF_CloseFont(font);
	SDL_FreeSurface(surf);
	return true;
}

void kfcRenderTex(SDL_Texture *tex, int x, int y)
{
	SDL_Rect dst;
	dst.x = x;
	dst.y = y;
	SDL_QueryTexture(tex, NULL, NULL, &dst.w, &dst.h);
	SDL_RenderCopy(ren, tex, NULL, &dst);
}

