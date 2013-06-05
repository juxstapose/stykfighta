#include "SDL.h"
#include "SDL_image.h"
#include <stdio.h>

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

SDL_Texture* load_image(SDL_Renderer *renderer, char* filename){
	SDL_Texture* tex = NULL;
	tex = IMG_LoadTexture(renderer, filename);
	if (tex == NULL) {
		printf("Failed to load image: %s %s\n", filename, IMG_GetError());
        exit(EXIT_FAILURE); 
    }
	return tex;
}

void apply_surface(int x, int y, SDL_Texture *tex, SDL_Renderer *rend) {
	//First we must create an SDL_Rect for the position of the image, as SDL
	//won't accept raw coordinates as the image's position
	SDL_Rect pos;
	pos.x = x;
	pos.y = y;
    
    //We also need to query the texture to get its width and height to use
    SDL_QueryTexture(tex, NULL, NULL, &pos.w, &pos.h);
	SDL_RenderCopy(rend, tex, NULL, &pos);
}

//TODO load meta file numbers into memory allocated array of structs
void sprite_load_meta_file(char* filename) {

}

//TODO accesses memory allocated array of structs for width and length and queries texture for indexed animation
void sprite_play_animation(int num, int frame, SDL_Texture *tex, SDL_Renderer *rend) {

}

//TODO
//load spritesheet
//load desired animation from spritesheet at position x,y on screen
//play animation at consistent framerate
//how do we determine consistent framerate?
//lock FPS at 60 frames or 30 frames per second?
//lock animation at 30 frames per second?
//keep track time pass between frames...when interval is met display frame
//do we incorporate the frame locking in the sprite class?
//are we ever going to have to animate a non-sprite...dunno...dont think so
//build sprite class that loads a sprite, animates a sprite
//load a meta file that tells height of sprites, #number of animations, width
//0 78 450
//1 82 734
//2 70 655
int main(int argc, char** argv){

    if (SDL_Init(SDL_INIT_EVERYTHING) == -1){
		printf("%s\n", SDL_GetError());
		return 1;
	}

    SDL_Renderer *renderer = NULL;
    SDL_Window *window = NULL;
    
	//Setup our window and renderer
	window = SDL_CreateWindow("Lesson 3", SDL_WINDOWPOS_CENTERED, 
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

    SDL_Texture *background = NULL;
    SDL_Texture *image = NULL;

    background = load_image(renderer, "res/background.png");
	image = load_image(renderer, "res/front_image.png");
	
    //Clear the window
	SDL_RenderClear(renderer);

	//Get the width and height from the texture so we know how much to move x,y by
	//to tile it correctly
	int bW, bH;
	SDL_QueryTexture(background, NULL, NULL, &bW, &bH);
	
    //We want to tile our background so draw it 4 times
	apply_surface(0, 0, background, renderer);
	apply_surface(bW, 0, background, renderer);
	apply_surface(0, bH, background, renderer);
	apply_surface(bW, bH, background, renderer);
    
    //Draw our image in the center of the window
	//We also need its width so query it as well
	int iW, iH;
	SDL_QueryTexture(image, NULL, NULL, &iW, &iH);
	int x = SCREEN_WIDTH / 2 - iW / 2;
	int y = SCREEN_HEIGHT / 2 - iH / 2;
    apply_surface(x, y, image, renderer);
	
    //Update the screen
	SDL_RenderPresent(renderer);
	SDL_Delay(2000);

	//Destroy the various items
	SDL_DestroyTexture(background);
	SDL_DestroyTexture(image);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	SDL_Quit();

    return 0;
}
