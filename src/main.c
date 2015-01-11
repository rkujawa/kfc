#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <sys/syslimits.h>

#define SCREEN_WIDTH 1440
#define SCREEN_HEIGHT 900

#define LETTERS 26

bool kfcTextToTex(const char *, SDL_Texture **, SDL_Color, int);
void kfcRenderChar(const char);
void kfcRenderTex(SDL_Texture *, int, int);
char kfcRandomLetter(void);
bool kfcInitSound(void);
void kfcQuitSound(void);
void logErrorSDL(const char *);

static const char *fontname = "/Library/Fonts/Arial Black.ttf";
static const char *vf_bravo = "audio/bravo.mp3";
static const char *vf_find = "audio/find.mp3";

static SDL_Color white = { 255, 255, 255 };
static SDL_Window *win;
static SDL_Renderer *ren;

static Mix_Music *v_bravo;
static Mix_Music *v_find;
static Mix_Music *v_alpha[LETTERS];

int main (int argc, char *argv[]) 
{
	char c;

	if (SDL_Init(SDL_INIT_EVERYTHING) != 0){
		logErrorSDL("SDL_Init Error");
		return 1;
	}

	win = SDL_CreateWindow("KFC!", 100, 100, SCREEN_WIDTH, 
	    SCREEN_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN_DESKTOP);	
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

	kfcInitSound();

	c = kfcRandomLetter();

	while (1) {
		SDL_Event e;

		SDL_RenderClear(ren);
		kfcRenderChar(c);
		SDL_RenderPresent(ren);

		if (SDL_PollEvent(&e)) {

			if (e.type == SDL_QUIT)
				break;

			if (e.type == SDL_KEYDOWN) { 
				if (e.key.keysym.sym == SDLK_ESCAPE)
					break;

				printf("%d\n", e.key.keysym.sym) ;
				if (e.key.keysym.sym == c) { 
					Mix_PlayMusic(v_bravo, 1);
					SDL_Delay(2000);
					c = kfcRandomLetter();

					SDL_RenderClear(ren);
					kfcRenderChar(c);
					SDL_RenderPresent(ren);

					Mix_PlayMusic(v_find, 1);
					SDL_Delay(3000);
					Mix_PlayMusic(v_alpha[c-97], 1);
				}
			}
		}
	}

	TTF_Quit();	
	SDL_DestroyRenderer(ren);
	SDL_DestroyWindow(win);
	kfcQuitSound();
	SDL_Quit();

	return 0;
}

bool kfcInitSound(void) 
{
	int i, rv, flags;
	char pathbuf[PATH_MAX];

	flags = MIX_INIT_MP3;

	rv = Mix_Init(flags);
	if ((rv & flags) != flags) {
		printf("fejlur: %s\n", Mix_GetError());
	}

	Mix_OpenAudio(16000, AUDIO_S16SYS, 1, 1024);
	v_find = Mix_LoadMUS(vf_find);
	v_bravo = Mix_LoadMUS(vf_bravo);

	for (i = 0; i < LETTERS; i++) {
		snprintf(pathbuf, PATH_MAX-1, "audio/%c.mp3", i+97);
		//printf("pathbuf: %s\n", pathbuf);
		v_alpha[i] = Mix_LoadMUS(pathbuf);
	}

	return true;
}

void kfcQuitSound(void)
{
	Mix_FreeMusic(v_find);
	Mix_FreeMusic(v_bravo);
}

void kfcRenderChar(const char c)
{
	static SDL_Texture *txttex;
	int iW, iH, x, y;

	char charBuf[2];
	charBuf[0] = c - 32;	
	charBuf[1] = '\0';

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

	surf = TTF_RenderText_Solid(font, msg, color);
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

/*
 * Idea stolen from Ryan Reich: 
 * http://stackoverflow.com/questions/2509679/how-to-generate-a-random-number-from-within-a-range
 */
char kfcRandomLetter(void) 
{
	unsigned long max = 'z' - 'a';
	unsigned long num_bins = (unsigned long) max + 1;
	unsigned long num_rand = (unsigned long) RAND_MAX + 1;
	unsigned long bin_size = num_rand / num_bins;
	unsigned long defect = num_rand % num_bins;

	long x;
	do {
		x = random();
	}
	while (num_rand - defect <= (unsigned long)x);

	return x/bin_size + 'a';
}

