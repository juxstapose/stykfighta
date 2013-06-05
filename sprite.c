#include "SDL.h"
#include "SDL_image.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "darray.h"
#include "sprite.h"

SDL_Texture* sprite_load_image(SDL_Renderer *renderer, char* filename){
	SDL_Texture* tex = NULL;
	tex = IMG_LoadTexture(renderer, filename);
	if (tex == NULL) {
		printf("Failed to load image: %s %s\n", filename, IMG_GetError());
        exit(EXIT_FAILURE); 
    }
	return tex;
}

void sprite_get_offset_wh_by_frame(int animation, int frame, DArray* meta_info, int *x_offset, int *y_offset, int* width, int* height) {
    *x_offset = 0;
    *y_offset = 0;
    int i = 0;
    int len = darray_length(meta_info);
    int row_offset = 0;
    int offset = 3;
    int num = 0;
    int index = 0;
    //printf("x offset: %i\n", *x_offset);
    for(i=0; i<len; i++) {
        //printf("%i == %i\n", i, row_offset);
        if(i == row_offset) {
            index = meta_info->items[row_offset];
            num = meta_info->items[row_offset + 1];
            *height = meta_info->items[row_offset+2];
            *y_offset = *y_offset + *height;
            //printf("index %i num %i\n", index, num);
            if(index == animation) { 
                //printf("find width and length\n");
                //find width and length
                int x = 0;
                for(x=offset; x<num+offset; x++) {
                    //get x offset
                    *x_offset = *x_offset + meta_info->items[row_offset+x];
                    //printf("x_off: %i\n", *x_offset);
                    //printf("x:%i == frame:%i\n", x, frame+offset); 
                    if(x == frame+offset) {
                        //printf("row_offset+x: %i\n", row_offset+x);
                        *width = meta_info->items[row_offset+x];
                        *x_offset = *x_offset - *width;
                        break;
                    }
                }
                *y_offset = *y_offset - *height;
                break;
            }
            row_offset = row_offset + num + offset;
        }
    } 
}

int sprite_get_frames(int animation, DArray *meta_info) {
    //get # frames
    int frames = 0;
    int i = 0;
    int len = darray_length(meta_info);
    int row_offset = 0;
    int num = 0;
    int index = 0;
    int offset = 3;
    for(i=0; i<len; i++) {
        if(i == row_offset) {
            index = meta_info->items[row_offset];
            num = meta_info->items[row_offset + 1];
            row_offset = row_offset + num + offset;
            if(index == animation) {
                frames = num;
                break;
            }
        }
    }
    return frames;
}

Sprite* sprite_create(int type, int animation, int x, int y, float delay, SDL_Texture *texture) {
    Sprite *sprite = (Sprite*)malloc(sizeof(Sprite));
    if(sprite == NULL) {
        printf("failed to alloc memory for sprite\n");
        exit(EXIT_FAILURE);
    }
    sprite->type = type;
    sprite->animation = animation;
    sprite->x = x;
    sprite->y = y;
    sprite->texture = texture;
    sprite->x_speed = 0;
    sprite->y_speed = 0;
    sprite->advance_frame = 0;
    sprite->last_update = SDL_GetTicks();
    sprite->current_frame = 0;
    sprite->delay = delay;
    sprite->key_state = 0;
    return sprite;
}

void sprite_destroy(Sprite *sprite) {
    free(sprite);
}

void sprite_update(Sprite* sprite, DArray *meta_info, SDL_Renderer *rend) {
    int frames = sprite_get_frames(sprite->animation, meta_info);
    //printf("animation: %i frames: %i\n", sprite->animation, frames);
    int i = 0;
    float now = SDL_GetTicks();
    printf("delay: %f - %f = %f > %f\n", now, sprite->last_update, now-sprite->last_update, sprite->delay);
    printf("advance frame: %i\n", sprite->advance_frame);
    //only advance frame index if enough elapsed time has passed
    if(now - sprite->last_update > sprite->delay && sprite->advance_frame) {
        char* display = "";
        if(sprite->animation == WALK) {
            display = "walk";
        }
        if(sprite->animation == TORNADO_KICK) {
            display = "tornado kick";
        }
        if(sprite->animation == SPINNING_BACK_KICK) {
            display = "spinning back kick";
        }
        printf("%s current_frame: %i frames: %i\n", display, sprite->current_frame, frames);
        sprite->current_frame = sprite->current_frame + 1;
        if(sprite->animation != WALK && sprite->animation != RUN) {
            if (sprite->current_frame == frames-1) {
                sprite->advance_frame = 0;
            }
        }
        sprite->current_frame = sprite->current_frame % frames;
        sprite->last_update = now;
    }
}

void sprite_render_frame(int animation, int current_frame, int x, int y, DArray *meta_info, SDL_Texture *tex, SDL_Renderer *rend) {
    
    //find position location of frame in spritesheet using parsed meta info
    int x_offset = 0;
    int y_offset = 0; 
    int width = 0;
    int height = 0;
    sprite_get_offset_wh_by_frame(animation, current_frame, meta_info, &x_offset, &y_offset, &width, &height);
    //printf("loading animation %i at frame %i using x:%i y:%i width:%i height:%i\n", animation, current_frame, x_offset, y_offset, width, height);	
     
    //First we must create an SDL_Rect for the position of the image, as SDL
	//won't accept raw coordinates as the image's position
    
    SDL_Rect pos;
	pos.x = x;
	pos.y = y;
    pos.w = width;
    pos.h = height;
    
    SDL_Rect clip;
    clip.x = x_offset;
    clip.y = y_offset;
    clip.w = width;
    clip.h = height;

    SDL_RenderCopy(rend, tex, &clip, &pos);
}

void sprite_load_meta_file(char* filename, DArray *meta_info) {
    FILE *file = fopen ( filename, "r" );
    char line[LINE_BUFF_SIZE];
    char* pch;
    memset(line, '\0', LINE_BUFF_SIZE);
    darray_init(meta_info);
    if ( file != NULL ) {
        while(fgets(line, sizeof line, file) != NULL) {
            int val = 0;
            fputs (line, stdout);
            pch = strtok (line," ,.-");
            val = atoi(pch);
            darray_add(meta_info, val);
            while(pch != NULL) {
                //printf ("%i\n",val);
                pch = strtok (NULL, " ,.-");
                if(pch != NULL) {
                    val = atoi(pch);
                    darray_add(meta_info, val);
                }
            } 
        }
        fclose(file);
   }
   else {
      perror(filename);
   } 
}

/**
int main(int argc, char** argv){
    char* filename = "animation_meta_info.txt";
    DArray meta_info;
    darray_init(&meta_info);
    sprite_load_meta_file(filename, &meta_info);
    printf("meta info\n");
    darray_print(&meta_info);
     
    int animation = 0;
    int frame = 0;
    animation = 2;
    frame = 7;
    int width = 0;
    int height = 0;
    int x_offset = 0;
    int y_offset = 0;
    sprite_get_offset_wh_by_frame(animation, frame, &meta_info, &x_offset, &y_offset, &width, &height);
    printf("animation: %i\n", animation); 
    printf("frame: %i\n", frame); 
    printf("width: %i\n", width); 
    printf("height: %i\n", height); 
    printf("x offset: %i\n", x_offset); 
    printf("y offset: %i\n", y_offset); 
    darray_destroy(&meta_info);

}
**/

int player_input(Sprite* player) {
    SDL_Event event;
    int quit  = 0;
    //Event Polling
    while (SDL_PollEvent(&event)){
        switch( event.type ){
            /* Look for a keypress */
            case SDL_KEYDOWN:
                printf("keydown\n");
                /* Check the SDLKey values and move change the coords */
                switch( event.key.keysym.sym ){
                    case SDLK_ESCAPE:
                        quit = 1;
                        break;
                    case SDLK_LEFT:
                        if( (player->animation == SPINNING_BACK_KICK || 
                             player->animation == TORNADO_KICK) && player->advance_frame == 1) {
                            printf("can't cancel...move animation playing\n");
                        } else {
                            player->animation = WALK;
                            printf("left key press down\n");
                            player->x_speed = -3;
                            player->advance_frame = 1;
                            player->key_state = SDLK_LEFT;
                        }
                        break;
                    case SDLK_RIGHT:
                        if( (player->animation == SPINNING_BACK_KICK || 
                             player->animation == TORNADO_KICK) && player->advance_frame == 1) {
                            printf("can't cancel...move animation playing\n");
                        } else {
                            player->animation = WALK;
                            printf("right key press down\n");
                            player->x_speed =  3;
                            player->advance_frame = 1;
                            player->key_state = SDLK_RIGHT;
                        }
                        break;
                    case SDLK_UP:
                        if( (player->animation == SPINNING_BACK_KICK || 
                             player->animation == TORNADO_KICK) && player->advance_frame == 1) {
                            printf("can't cancel...move animation playing\n");
                        } else {
                            player->animation = WALK;
                            printf("up key press down\n");
                            player->y_speed = -3;
                            player->advance_frame = 1;
                            player->key_state = SDLK_UP;
                        }
                        break;
                    case SDLK_DOWN:
                        if( (player->animation == SPINNING_BACK_KICK || 
                             player->animation == TORNADO_KICK) && player->advance_frame == 1) {
                            printf("can't cancel...move animation playing\n");
                        } else {
                            player->animation = WALK;
                            printf("down key press down\n");
                            player->y_speed =  3;
                            player->advance_frame = 1;
                            player->key_state = SDLK_DOWN;
                        }
                        break;
                    case SDLK_z:
                        player->animation = SPINNING_BACK_KICK;
                        player->current_frame = 0;
                        player->advance_frame = 1;
                        player->key_state = SDLK_z;
                        break;
                    case SDLK_x:
                        player->animation = TORNADO_KICK;
                        player->current_frame = 0;
                        player->advance_frame = 1;
                        player->key_state = SDLK_x;
                        break;
                    default:
                        break;
                }
                break;
            case SDL_KEYUP:
                printf("keyup\n");
                switch( event.key.keysym.sym ){
                    case SDLK_LEFT:
                        printf("left key press up\n");
                        if( player->x_speed < 0 ) {
                            player->x_speed = 0;
                        }
                        printf("last button pressed: %i\n", player->key_state);
                        printf("sdlk left: %i\n", SDLK_LEFT);
                        if(player->key_state == SDLK_LEFT) {
                            player->advance_frame = 0;
                        }
                        break;
                    case SDLK_RIGHT:
                        printf("right key press up\n");
                        if( player->x_speed > 0 ) {
                            player->x_speed = 0;
                        }
                        if(player->key_state == SDLK_RIGHT) {
                            player->advance_frame = 0;
                        }
                        break;
                    case SDLK_UP:
                        printf("up key press up\n");
                        if( player->y_speed < 0 ) {
                            player->y_speed = 0;
                        }
                        if(player->key_state == SDLK_UP) {
                            player->advance_frame = 0;
                        }
                        break;
                    case SDLK_DOWN:
                        printf("down key press up\n");
                        if( player->y_speed > 0 ) {
                            player->y_speed = 0;
                        }
                        if(player->key_state == SDLK_DOWN) {
                            player->advance_frame = 0;
                        }
                        break;
                    default:
                        break;
                }
                break;
            
            //If user closes the window
            case SDL_QUIT:
                printf("window close\n");
                quit = 1;
                break;
            
            default:
                break;
        }
    }
    
    if( (player->animation == SPINNING_BACK_KICK || 
         player->animation == TORNADO_KICK) && player->advance_frame == 1) {
        printf("can't move...move animation playing\n");
    } else {    
        if(player->animation == WALK || player->animation == RUN) {
            player->x += player->x_speed;
            player->y += player->y_speed;
        }
    }
    return quit;
}



int main(int argc, char** argv){
    printf("hello world\n");
    if (SDL_Init(SDL_INIT_EVERYTHING) == -1){
		printf("%s\n", SDL_GetError());
		return 1;
	}
    printf("sdl init success\n");

    SDL_Renderer *renderer = NULL;
    SDL_Window *window = NULL;
    
	//Setup our window and renderer
	window = SDL_CreateWindow("Stick Fighter", SDL_WINDOWPOS_CENTERED, 
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

    SDL_Texture *image = NULL;

    //background = sprite_load_image(renderer, "res/white_background.png");
	image = sprite_load_image(renderer, "res/output.png");
    printf("after sprite load image\n");
        
    //load the meta info
    char* filename = "animation_meta_info.txt";
    DArray meta_info;
    darray_init(&meta_info);
    sprite_load_meta_file(filename, &meta_info);
    darray_print(&meta_info);

    float interval = 0.0f;
    float start = 0.0f;
    int quit = 0;
    
    float delay = 1000.0f / FPS;
    Sprite* player = sprite_create(PLAYER, WALK, 200, 100, delay, image);
    Sprite* enemy = sprite_create(ENEMY, WALK, 0, 0, delay, image);
    
    //set white background
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

	while (!quit){
		start = SDL_GetTicks();
        
        quit = player_input(player);
        printf("quit: %i\n", quit); 
        //Rendering
		SDL_RenderClear(renderer);
        
        //update animation frame
        sprite_update(player, &meta_info, renderer);
        sprite_update(enemy, &meta_info, renderer);
        
        //draw sprite
        sprite_render_frame(player->animation, player->current_frame, player->x, player->y, &meta_info, player->texture, renderer);
        sprite_render_frame(enemy->animation, enemy->current_frame, enemy->x, enemy->y, &meta_info, enemy->texture, renderer);

		//Update the screen
		SDL_RenderPresent(renderer);
         
        interval = SDL_GetTicks() - start;
        
        if(interval > 0) {
            float fps = 1.0f / (interval / 1000.0f);
            //printf("%f fps\n", fps);
        }
           
	}
    
    //SDL_Delay(4000);

	//Destroy the various items
    sprite_destroy(player);
    sprite_destroy(enemy);
    darray_destroy(&meta_info);
	//SDL_DestroyTexture(background);
	SDL_DestroyTexture(image);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
    
    printf("after destory text rend win\n");

	SDL_Quit();

    return 0;
}

