#include <stdlib.h>
#include <unistd.h>
#include "SDL.h"
#include "SDL_image.h"

main(int argc, char *argv[])
{
	// init SDL
	if ( SDL_Init( SDL_INIT_VIDEO ) < 0 ) {
		fprintf(stderr, "Unable to init SDL: %s\n", SDL_GetError());
		exit(1);
	}
	atexit(SDL_Quit);

	// Set mode
	SDL_Surface *screen;

	screen = SDL_SetVideoMode(800, 800, 32, SDL_SWSURFACE);
	if ( screen == NULL ) {
		fprintf(stderr, "Unable to set 640x480 video: %s\n", SDL_GetError());
		exit(1);
	}
	printf("Mode set\n");
	
	// Load image
	SDL_Surface *image;
	const char* file = "texture.png";

	/* Load the BMP file into a surface */
	image = IMG_Load(file);
	if ( image == NULL ) {
		fprintf(stderr, "Couldn't load %s: %s\n", file, SDL_GetError());
		return 1;
	}
	printf("Image loaded\n");
	
	// draw image onto surface
	SDL_Rect dest;
    dest.x = 10;
    dest.y = 10;
    dest.w = image->w;
    dest.h = image->h;
    SDL_BlitSurface(image, NULL, screen, &dest);

    /* Update the changed portion of the screen */
    SDL_UpdateRects(screen, 1, &dest);
	
	sleep(3);

	return 0;
}
