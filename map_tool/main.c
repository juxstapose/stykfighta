#include "SDL.h"
#include "SDL_image.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768

int main(int argc, char** argv){
    printf("hello world\n");

    if (SDL_Init(SDL_INIT_EVERYTHING) == -1){
		printf("%s\n", SDL_GetError());
		return 1;
	}
    
    SDL_Renderer *renderer = NULL;
    SDL_Window *window = NULL;
    
	//Setup our window and renderer
	window = SDL_CreateWindow("MAP TOOL", SDL_WINDOWPOS_CENTERED, 
                                               SDL_WINDOWPOS_CENTERED, 
                                               SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	if (window == NULL){
		printf("%s\n", SDL_GetError());
		return 2;
	}
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (renderer == NULL){
		printf("%s\n", SDL_GetError());
		return 3;
	}
    
    //button that loads a background map
    //button has a text box that describes path to background image
    //loading an image to display loads image on top of eachother
    //loading an image is in a tool bar on left side...background image loads to the right
    //loading an object image is also an option
    //object image has a meta file that describes it depth,height, and width
    //an option to draw a shaded box on the map
    //an option to remove shaded box on the map
    //drawing a shaded box writes a meta file

}
