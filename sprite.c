#include "SDL.h"
#include "SDL_image.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "log.h"
#include "main.h"
#include "util.h"
#include "darray.h"
#include "pf_list.h"
#include "pathfinding.h"
#include "behaviors.h"
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
    sprite->last_animation = animation;
    sprite->x = x;
    sprite->y = y;
    sprite->texture = texture;
    sprite->x_speed = 1;
    sprite->y_speed = 1;
    sprite->advance_frame = 0;
    sprite->last_update = SDL_GetTicks();
    sprite->delay = delay;
    sprite->key_state = 0;
    sprite->right_key = 0;
    sprite->left_key = 0;
    sprite->up_key = 0;
    sprite->down_key = 0;
    sprite->wander_direction = 90;
    sprite->bounding_box.x = 0;
    sprite->bounding_box.y = 0;
    sprite->bounding_box.w = 0;
    sprite->bounding_box.h = 0;
    sprite->state = WANDER;
    sprite->attack_duration_start = SDL_GetTicks();
    sprite->wander_start = SDL_GetTicks();
    sprite->stall_start = SDL_GetTicks();
    sprite->attack_duration = 6000;
    sprite->wander_duration = 4000;
    sprite->stall_duration = 2000;
    sprite->location = node_create(0,0);
    sprite->seek_count = 0;
    sprite->seek_length = 0;
    sprite->flee_loc_x = 0;
    sprite->flee_loc_y = 0;
    sprite->moving_right = 0;
    sprite->moving_left = 0;
    sprite->scroll = 0;
    sprite->health = 100;
    sprite->max_health = 100;

    int i = 0;
    for(i=0; i<MAX_ANIMATIONS; i++) {
        sprite->current_frame[i] = 0;
    }
    for(i=0; i<MAX_ANIMATIONS; i++) {
        sprite->animation_count[i] = 0;
    }
    for(i=0; i<MAX_ANIMATIONS; i++) {
        sprite->last_animation_count[i] = 0;
    }
    sprite->direction = SDL_FLIP_NONE;
    return sprite;
}

void sprite_destroy(Sprite *sprite) {
    node_destroy(sprite->location);
    free(sprite);
}

void sprite_update(Sprite* sprite, DArray *meta_info, SDL_Renderer *rend) {
    int frames = sprite_get_frames(sprite->animation, meta_info);
    //printf("animation: %i frames: %i\n", sprite->animation, frames);
    //int i = 0;
    float now = SDL_GetTicks();
    //printf("delay: %f - %f = %f > %f\n", now, sprite->last_update, now-sprite->last_update, sprite->delay);
    //only advance frame index if enough elapsed time has passed
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
    if(sprite->animation == FRONT_KICK) {
        display = "front kick";
    }
    if(sprite->animation == HIT) {
        display = "hit";
    }
    //printf("%s current_frame: %i frames: %i advance_frame:%i \n", display, sprite->current_frame[sprite->animation], frames, sprite->advance_frame);
    if(now - sprite->last_update > sprite->delay && sprite->advance_frame) {
        sprite->current_frame[sprite->animation] = sprite->current_frame[sprite->animation] + 1;
        if(sprite->animation != WALK && sprite->animation != RUN) {
            if (sprite->current_frame[sprite->animation] == frames-1) {
                sprite->advance_frame = 0;
            }
        }
        if(sprite->current_frame[sprite->animation] == frames-1) {
            sprite->animation_count[sprite->animation]++;
        }
        sprite->current_frame[sprite->animation] = sprite->current_frame[sprite->animation] % frames;
        sprite->last_update = now;
    }
}

void sprite_update_bounding_box(Sprite *sprite, DArray *meta_info) {
    
    int x_offset = 0;
    int y_offset = 0; 
    int width = 0;
    int height = 0;
    sprite_get_offset_wh_by_frame(sprite->animation, sprite->current_frame[sprite->animation], meta_info, &x_offset, &y_offset, &width, &height);

    sprite->bounding_box.x = sprite->x; 
    sprite->bounding_box.y = sprite->y;
    sprite->bounding_box.w = width;
    sprite->bounding_box.h = height;

}

void sprite_render_frame(float bg_width, float bg_height, Sprite *sprite, DArray *meta_info, SDL_Renderer *rend, int draw_bounding_box) {
    
    //find position location of frame in spritesheet using parsed meta info
    int x_offset = 0;
    int y_offset = 0; 
    int width = 0;
    int height = 0;
    
    //printf("render: sprite->animation:%i current frame:%i\n", sprite->animation, sprite->current_frame[sprite->animation]);
    sprite_get_offset_wh_by_frame(sprite->animation, sprite->current_frame[sprite->animation], meta_info, &x_offset, &y_offset, &width, &height);
    
    //printf("loading animation %i at frame %i using x:%i y:%i width:%i height:%i\n", animation, current_frame, x_offset, y_offset, width, height);	
     
    //First we must create an SDL_Rect for the position of the image, as SDL
	//won't accept raw coordinates as the image's position
    
    //boundry check
    if(sprite->x < 0) {
        sprite->x = 0;
    }
    //TODO width of background image
    //if(sprite->x > SCREEN_WIDTH - width) {
    //    sprite->x = SCREEN_WIDTH - width;
    //}
    //if(sprite->x > bg_width - width) {
    //    sprite->x = bg_height - width;
    //}
    if(sprite->y < 200) {
        sprite->y = 200;
    }
    //if(sprite->y > SCREEN_HEIGHT - height) {
    //    sprite->y = SCREEN_HEIGHT - height; 
    //}
    //if(sprite->y > bg_height - height) {
    //    sprite->y = bg_height - height; 
    //}
    //printf("plot (%i,%i)\n", sprite->x, sprite->y);
    
    if(draw_bounding_box) {
        Uint8 r = 0;
        Uint8 g = 255;
        Uint8 b = 0;
        Uint8 a = 0;
        int result = SDL_SetRenderDrawColor(rend, r, g, b, a);
        if(result != 0) {
            printf("render color failed: %s\n", SDL_GetError());
            exit(1);
        }
        result = SDL_RenderDrawRect(rend, &sprite->bounding_box);
        if(result != 0) {
            printf("render rect failed: %s\n", SDL_GetError());
            exit(1);
        }
        r = 255;
        g = 255;
        b = 255;
        a = 255;
        SDL_SetRenderDrawColor(rend, r, g, b, a);
    }


    SDL_Rect pos;
	pos.x = sprite->x;
	pos.y = sprite->y;
    pos.w = width;
    pos.h = height;
    
    SDL_Rect clip;
    clip.x = x_offset;
    clip.y = y_offset;
    clip.w = width;
    clip.h = height;

    /**
    Uint8 r = 0;
    Uint8 g = 0;
    Uint8 b = 0;
    int result = SDL_GetTextureColorMod(sprite->texture, &r, &g, &b);
    printf("get color mod: r: %i, g: %i, b: %i\n", r, g, b);
    result =  SDL_SetTextureColorMod(sprite->texture,
                                        0,
                                        0,
                                        255);
    **/
    //SDL_RenderCopy(rend, sprite->texture, &clip, &pos);
    //SDL_RenderCopyEx(rend, sprite->texture, &clip, &pos, 0, NULL, SDL_FLIP_HORIZONTAL);
    SDL_RenderCopyEx(rend, sprite->texture, &clip, &pos, 0, NULL, sprite->direction);
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

int player_input(Sprite* player, DArray *meta_info) {
    SDL_Event event;
    int quit  = 0;
    //Event Polling
    while (SDL_PollEvent(&event)){
        switch( event.type ){
            /* Look for a keypress */
            case SDL_KEYDOWN:
                //printf("keydown\n");
                /* Check the SDLKey values and move change the coords */
                switch( event.key.keysym.sym ){
                    case SDLK_ESCAPE:
                        quit = 1;
                        break;
                    case SDLK_LEFT:
                        if( (player->animation == SPINNING_BACK_KICK || 
                             player->animation == TORNADO_KICK) && player->advance_frame == 1) {
                            //printf("can't cancel...move animation playing\n");
                        } else {
                            player->animation = WALK;
                            //printf("left key press down\n");
                            player->x_speed = -3;
                            player->advance_frame = 1;
                            player->key_state = SDLK_LEFT;
                            player->left_key = 1;
                            player->direction = SDL_FLIP_HORIZONTAL;
                            player->moving_left = 1;
                        }
                        break;
                    case SDLK_RIGHT:
                        if( (player->animation == SPINNING_BACK_KICK || 
                             player->animation == TORNADO_KICK) && player->advance_frame == 1) {
                            //printf("can't cancel...move animation playing\n");
                        } else {
                            player->animation = WALK;
                            //printf("right key press down\n");
                            player->x_speed =  3;
                            player->advance_frame = 1;
                            player->key_state = SDLK_RIGHT;
                            player->right_key = 1;
                            player->direction = SDL_FLIP_NONE;
                            player->moving_right = 1;
                        }
                        break;
                    case SDLK_UP:
                        if( (player->animation == SPINNING_BACK_KICK || 
                             player->animation == TORNADO_KICK) && player->advance_frame == 1) {
                            //printf("can't cancel...move animation playing\n");
                        } else {
                            player->animation = WALK;
                            //printf("up key press down\n");
                            player->y_speed = -3;
                            player->advance_frame = 1;
                            player->key_state = SDLK_UP;
                            player->up_key = 1;
                        }
                        break;
                    case SDLK_DOWN:
                        if( (player->animation == SPINNING_BACK_KICK || 
                             player->animation == TORNADO_KICK) && player->advance_frame == 1) {
                            //printf("can't cancel...move animation playing\n");
                        } else {
                            player->animation = WALK;
                            //printf("down key press down\n");
                            player->y_speed =  3;
                            player->advance_frame = 1;
                            player->key_state = SDLK_DOWN;
                            player->down_key = 1;
                        }
                        break;
                    case SDLK_z:
                        player->animation = SPINNING_BACK_KICK;
                        player->current_frame[player->animation] = 0;
                        player->advance_frame = 1;
                        player->key_state = SDLK_z;
                        break;
                    case SDLK_x:
                        player->animation = TORNADO_KICK;
                        player->current_frame[player->animation] = 0;
                        player->advance_frame = 1;
                        player->key_state = SDLK_x;
                        break;
                    default:
                        break;
                }
                break;
            case SDL_KEYUP:
                //printf("keyup\n");
                switch( event.key.keysym.sym ){
                    case SDLK_LEFT:
                        //printf("left key press up\n");
                        if( player->x_speed < 0 ) {
                            player->x_speed = 0;
                        }
                        //printf("last button pressed: %i\n", player->key_state);
                        //printf("sdlk left: %i\n", SDLK_LEFT);
                        //if(player->key_state == SDLK_LEFT) {
                        //    player->advance_frame = 0;
                        //}
                        if(player->left_key == 1) {
                            player->left_key = 0;
                        }
                        if(player->right_key == 0 && player->up_key == 0 && player->down_key == 0) {
                            player->advance_frame = 0;
                        }
                        player->moving_left = 0;
                        break;
                    case SDLK_RIGHT:
                        //printf("right key press up\n");
                        if( player->x_speed > 0 ) {
                            player->x_speed = 0;
                        }
                        //if(player->key_state == SDLK_RIGHT) {
                        //    player->advance_frame = 0;
                        //}
                        if(player->right_key == 1) {
                            player->right_key = 0;
                        }
                        if(player->left_key == 0 && player->up_key == 0 && player->down_key == 0) {
                            player->advance_frame = 0;
                        }
                        player->moving_right = 0;
                        break;
                    case SDLK_UP:
                        //printf("up key press up\n");
                        if( player->y_speed < 0 ) {
                            player->y_speed = 0;
                        }
                        //if(player->key_state == SDLK_UP) {
                        //    player->advance_frame = 0;
                        //}
                        if(player->up_key == 1) {
                            player->up_key = 0;
                        }
                        if(player->left_key == 0 && player->right_key == 0 && player->down_key == 0) {
                            player->advance_frame = 0;
                        }
                        break;
                    case SDLK_DOWN:
                        //printf("down key press up\n");
                        if( player->y_speed > 0 ) {
                            player->y_speed = 0;
                        }
                        //if(player->key_state == SDLK_DOWN) {
                        //    player->advance_frame = 0;
                        //}
                        if(player->down_key == 1) {
                            player->down_key = 0;
                        }
                        if(player->left_key == 0 && player->right_key == 0 && player->up_key == 0) {
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
            //boundry check
            int x_offset = 0;
            int y_offset = 0;
            int width = 0;
            int height = 0;
            sprite_get_offset_wh_by_frame(player->animation, player->current_frame[player->animation], meta_info, &x_offset, &y_offset, &width, &height);
            
            if(player->x < 0) {
                player->x = 0;
            }
            if(player->x > SCREEN_WIDTH - width) {
                player->x = SCREEN_WIDTH - width;
            }
            if(player->y < 200) {
                player->y = 200;
            }
            if(player->y > SCREEN_HEIGHT - height) {
                player->y = SCREEN_HEIGHT - height; 
            }
        }
    }
    
    
    return quit;
}

void world_to_pixel(float bg_width, float bg_height, float world_width, float world_height, int x, int y, int *dest_x, int *dest_y) {
    //transform world space to pixel space
    //TODO: change from screen to background image height and width
    //float wp_scale_x = SCREEN_WIDTH / WORLD_WIDTH;
    //float wp_scale_y = SCREEN_HEIGHT / WORLD_HEIGHT;
    
    float wp_scale_x = bg_width / world_width;
    float wp_scale_y = bg_height / world_height;
    

    *dest_x = x * wp_scale_x;
    *dest_y = y * wp_scale_y;
}

//void pixel_to_world(int x, int y, int *dest_x, int *dest_y) {
void pixel_to_world(float bg_width, float bg_height, float world_width, float world_height, int x, int y, int *dest_x, int *dest_y) {
    //transform pixel space to world space
    //TODO: change from screen to background image height and width
    float pw_scale_x = world_width / bg_width;
    float pw_scale_y = world_height / bg_height;
    *dest_x = x * pw_scale_x;
    *dest_y = y * pw_scale_y;
}

void moveto_coordinates(Sprite *sprite, int dest_x, int dest_y) {

    //if the current x is lest than the destination increase x by speed offset
    //only add speed if not within destination range
    int last_x = sprite->x;
    //int last_y = sprite->y;
    
    if(is_within_int(sprite->x, dest_x, sprite->x_speed) == 0) { 
        if(sprite->x < dest_x) {
            sprite->x+=sprite->x_speed;
        } else if(sprite->x > dest_x){
            sprite->x-=sprite->x_speed;
        }
    }
    
    if(is_within_int(sprite->y, dest_y, sprite->y_speed) == 0) {
        //printf("not within y sprite->y:%i < dest_y:%i y_speed:%i\n", sprite->y, dest_y, sprite->y_speed);
        if(sprite->y < dest_y) {
            //printf("increase\n");
            sprite->y+=sprite->y_speed;
        } else if(sprite->y > dest_y){
            //printf("decrease\n");
            sprite->y-=sprite->y_speed;
        }
    }

    int diff_x = sprite->x - last_x;
    //int diff_y = sprite->y - last_y;
    
    //are we moving left or right....if left we flip sprite
    if(diff_x > 0) {
        sprite->direction = SDL_FLIP_NONE;
    } else {
        sprite->direction = SDL_FLIP_HORIZONTAL;
    }

}

int within_coordinates(Sprite *sprite, int dest_x, int dest_y) {
    if(is_within_int(sprite->x, dest_x, sprite->x_speed) == 1 && 
       is_within_int(sprite->y, dest_y, sprite->y_speed) == 1) {
        return 1;
    }
    return 0;
}

void wander(Sprite *enemy) {

    int wander_test = SDL_GetTicks() - enemy->wander_start;
    printf("wander test:%i < duration: %i\n", wander_test, enemy->wander_duration);
    if(wander_test < enemy->wander_duration) {
        printf("wandering\n");
        enemy->state = WANDER;
        behaviors_generate_location(enemy->location, &enemy->wander_direction);
    } else {
        printf("not wandering\n");
    }
}

void wander_attack(Sprite *player, Sprite *enemy, DArray *meta_info, int num_attacks) {
    int near = behaviors_is_target_near(player->location, enemy->location, 25);
    if(enemy->state != SEEK && near == 0) {
        behaviors_generate_location(enemy->location, &enemy->wander_direction);
        printf("attack: WANDER\n");
        enemy->state = WANDER;
    }
    if(enemy->state != SEEK && near == 1) {
        Node *result = node_create(0,0);
        printf("attack: enemy->location->x:%i enemy->location->y:%i\n", enemy->location->coords->x, enemy->location->coords->y);
        printf("attack: player->location->x:%i player->location->y:%i\n", player->location->coords->x, player->location->coords->y);
        behaviors_seek(enemy->location, player->location, result, &enemy->seek_count, &enemy->seek_length);
        enemy->location->coords->x = result->coords->x;
        enemy->location->coords->y = result->coords->y;
        printf("attack: after enemy->location->x:%i enemy->location->y:%i\n", enemy->location->coords->x, enemy->location->coords->y);
        node_destroy(result);
        enemy->state = SEEK;
    }


    if(enemy->state == SEEK && enemy->seek_count < enemy->seek_length-4) {
        Node *result = node_create(0,0);
        printf("enemy->location->x:%i enemy->location->y:%i\n", enemy->location->coords->x, enemy->location->coords->y);
        behaviors_seek(enemy->location, player->location, result, &enemy->seek_count, &enemy->seek_length);
        enemy->location->coords->x = result->coords->x;
        enemy->location->coords->y = result->coords->y;
        printf("after enemy->location->x:%i enemy->location->y:%i\n", enemy->location->coords->x, enemy->location->coords->y);
        printf("count: %i out of length %i\n", enemy->seek_count, enemy->seek_length);
        node_destroy(result);
    }
    
    if(enemy->state == SEEK && enemy->seek_count == enemy->seek_length-4) {
        printf("attack: enemy->x: %i enemy->y: %i\n", enemy->x, enemy->y);
        //attack player
        //change animation to attack move
        printf("CHECK %i\n", enemy->animation_count[TORNADO_KICK]);
        enemy->animation = TORNADO_KICK;
        enemy->advance_frame = 1;
    }

    if(enemy->animation_count[TORNADO_KICK] == num_attacks) {
        printf("KICKS\n");
        printf("attack: after 3 attacks\n");
        enemy->advance_frame = 0;
        enemy->animation_count[TORNADO_KICK] = 0;
        enemy->state = ATTACK;

    }
    
    //to complete an ai function it would have to post that it was done
    //could send in a variable to modify its result would update state



}

//TODO: node_create(0,0) is a problem because it doesnt pay attention to boundries
//0,0 is absolute 0,0 position on the map not shaded out area
//need to create a tool that shades out boundries...by creating a map meta file that lists rectangles that are not walkable
//rectangles are shaded objects that have height, width, and depth that are jumpable and also boundries unless you jump on top
void wander_stall_attack(Sprite *player, Sprite *enemy, DArray *meta_info, int num_attacks) {
    /**
    int x_offset = 0;
    int y_offset = 0;
    int width = 0;
    int height = 0;
    sprite_get_offset_wh_by_frame(enemy->animation, enemy->current_frame, meta_info, &x_offset, &y_offset, &width, &height);
    **/

    printf("player->x:%i player->y:%i node = player x:%i player y:%i enemy:%i enemy:%i\n", player->x, player->y, player->location->coords->x, player->location->coords->y, enemy->location->coords->x, enemy->location->coords->y);
    int near = behaviors_is_target_near(player->location, enemy->location, 25);
    printf("is target near: %i\n", near);
    printf("count: %i out of length %i\n", enemy->seek_count, enemy->seek_length);
    printf("tornado animation count: %i\n", enemy->animation_count[TORNADO_KICK]);
    
    int wander_test = SDL_GetTicks() - enemy->wander_start;

    //have sprite wander on screen
    //generates new coordinates in world space
    printf("WANDER test: %i\n", wander_test);
    if(wander_test < enemy->wander_duration) {
        if(near == 0 && enemy->state == WANDER) {
            printf("near == 0\n");
            behaviors_generate_location(enemy->location, &enemy->wander_direction);
            printf("WANDER\n");
            enemy->state = WANDER;
        } else if(near == 1 && enemy->state == WANDER) {
            printf("near == 1 and WANDER\n");
            Node *result = node_create(0,0);
            printf("enemy->location->x:%i enemy->location->y:%i\n", enemy->location->coords->x, enemy->location->coords->y);
            behaviors_seek(enemy->location, player->location, result, &enemy->seek_count, &enemy->seek_length);
            enemy->location->coords->x = result->coords->x;
            enemy->location->coords->y = result->coords->y;
            printf("after enemy->location->x:%i enemy->location->y:%i\n", enemy->location->coords->x, enemy->location->coords->y);
            node_destroy(result);
            enemy->state = SEEK;
        }
    } else if(enemy->state != SEEK && enemy->state != ATTACK && enemy->state != FLEE && enemy->state != STALL) {
        printf("make STALL\n");
        enemy->state = STALL;
        enemy->stall_start = SDL_GetTicks();
        enemy->advance_frame = 0;
    } else {
        printf("STUCK BLOWN UP state:%i\n", enemy->state);
    }

    if(enemy->state == STALL) {
        int stall_test = SDL_GetTicks() - enemy->stall_start;
        printf("STALL test: %i\n", stall_test);
        if(stall_test > enemy->stall_duration) { 
            enemy->state = WANDER;
            enemy->wander_start = SDL_GetTicks();
            enemy->advance_frame = 1;
        }
    }
    
    if(enemy->state == SEEK && enemy->seek_count == enemy->seek_length-4) {
        printf("reached destination\n");
        printf("enemy->x: %i enemy->y: %i\n", enemy->x, enemy->y);
        //attack player
        //change animation to attack move
        enemy->animation = TORNADO_KICK;
        enemy->advance_frame = 1;
    }
    if(enemy->animation_count[TORNADO_KICK] == num_attacks) {
        enemy->advance_frame = 0;
        enemy->animation_count[TORNADO_KICK] = 0;
        printf("after 3 attacks\n");
        enemy->state = ATTACK;
    }
    if(enemy->state == ATTACK) {
        printf("flee\n");
        Node *result = node_create(0,0);
        enemy->flee_loc_x = player->location->coords->x - 25;
        enemy->flee_loc_y = player->location->coords->y - 25;
        
        Node *target_node = node_create(enemy->flee_loc_x,enemy->flee_loc_y);
        behaviors_seek(enemy->location, target_node, result, &enemy->seek_count, &enemy->seek_length);
        enemy->location->coords->x = result->coords->x;
        enemy->location->coords->y = result->coords->y;
        enemy->animation = WALK;
        enemy->advance_frame = 1;
        node_destroy(target_node);
        node_destroy(result);
        enemy->state = FLEE;
    }
    if(enemy->state == FLEE && enemy->seek_count < enemy->seek_length-1) {
        printf("FLEE count %i length %i\n", enemy->seek_count, enemy->seek_length);
        Node *result = node_create(0,0);
        Node *target_node = node_create(enemy->flee_loc_x,enemy->flee_loc_y);
        behaviors_seek(enemy->location, target_node, result, &enemy->seek_count, &enemy->seek_length);
        enemy->location->coords->x = result->coords->x;
        enemy->location->coords->y = result->coords->y;
        enemy->animation = WALK;
        enemy->advance_frame = 1;
        node_destroy(target_node);
        node_destroy(result);
    }
    if(enemy->state == FLEE && enemy->seek_count == enemy->seek_length-1) {
        enemy->state = WANDER;
        printf("FLEE NOW WANDER\n");
    }    
    if(enemy->state == SEEK && enemy->seek_count < enemy->seek_length-4) {
        Node *result = node_create(0,0);
        printf("enemy->location->x:%i enemy->location->y:%i\n", enemy->location->coords->x, enemy->location->coords->y);
        behaviors_seek(enemy->location, player->location, result, &enemy->seek_count, &enemy->seek_length);
        enemy->location->coords->x = result->coords->x;
        enemy->location->coords->y = result->coords->y;
        printf("after enemy->location->x:%i enemy->location->y:%i\n", enemy->location->coords->x, enemy->location->coords->y);
        printf("count: %i out of length %i\n", enemy->seek_count, enemy->seek_length);
        node_destroy(result);
    }

}

int sprite_check_collision(Sprite *sprite1, Sprite *sprite2) {
    //if any of the sides from 1 are outside of 2
    //if bottom1 is less than or equal to top2
    if(sprite1->bounding_box.y + sprite1->bounding_box.h <= sprite2->bounding_box.y) {
        return 0;
    }
    //if top1 is greater than or equal to bottom2
    if(sprite1->bounding_box.y >= sprite2->bounding_box.y + sprite2->bounding_box.h) {
        return 0;
    }
    //if right1 is less than or equal to left2
    if(sprite1->bounding_box.x + sprite1->bounding_box.w <= sprite2->bounding_box.x) {
        return 0;
    }
    //if left1 greater than or equal to right2
    if(sprite1->bounding_box.x >= sprite2->bounding_box.x + sprite2->bounding_box.w) {
        return 0;
    }
    //if none of the sides from 1 are outside of 2
    return 1;
}

int sprite_is_attack(int animation) {
    if(animation == FRONT_KICK || 
       animation == TORNADO_KICK || 
       animation == SPINNING_BACK_KICK) {
        return 1;
    }
    return 0;
}

void sprite_handle_collision(Sprite *sprite1, Sprite *sprite2, DArray *meta_info) {
    if(sprite_is_attack(sprite2->animation)) {
        sprite1->animation = HIT;
        if(sprite1->last_animation != sprite1->animation) {
            sprite1->last_animation = sprite1->animation;
            if(sprite1->health - 15 < 0) {
                sprite1->health = 0;
            } else {
                sprite1->health -= 15;
            }
        }
        sprite1->advance_frame = 0;
        int frames = sprite_get_frames(sprite1->animation, meta_info);
        //printf("animation count: %i\n", sprite2->animation_count[TORNADO_KICK]);
        //printf("last animation count: %i\n", sprite2->last_animation_count[TORNADO_KICK]);
        //log_msg(LOG_DEBUG, "animation count: %i\n", sprite2->animation_count[TORNADO_KICK]);
        //log_msg(LOG_DEBUG, "last animation count: %i\n", sprite2->last_animation_count[TORNADO_KICK]);
        //log_msg(LOG_DEBUG, "sprite 2 current frame: %i\n", sprite2->current_frame[TORNADO_KICK]);
        if(sprite2->last_animation_count[TORNADO_KICK] != sprite2->animation_count[TORNADO_KICK]) {
            //log_msg(LOG_DEBUG, "there was a change: last %i != now %i\n", 
            //        sprite2->last_animation_count[TORNADO_KICK],
            //        sprite2->animation_count[TORNADO_KICK]);
            //printf("there was a change: last %i != now %i\n", 
            //printf("player current frame: %i\n", sprite1->current_frame);
            sprite1->current_frame[sprite1->animation]++;
            sprite1->current_frame[sprite1->animation] = sprite1->current_frame[sprite1->animation] % frames;
            if(sprite1->health - 15 < 0) {
                sprite1->health = 0;
            } else {
                sprite1->health -= 15;
            }
        }
        //log_msg(LOG_DEBUG, "HIT player current frame: %i\n", sprite1->current_frame[sprite1->animation]);
        sprite2->last_animation_count[TORNADO_KICK] = sprite2->animation_count[TORNADO_KICK];
    } 
}

void boundry_check(float bg_width, float bg_height, Sprite *sprite, DArray *meta_info, int *x, int *y) {
    int x_offset = 0;
    int y_offset = 0; 
    int width = 0;
    int height = 0;
    sprite_get_offset_wh_by_frame(sprite->animation, sprite->current_frame[sprite->animation], meta_info, &x_offset, &y_offset, &width, &height);

    if(*x < 0) {
        *x = 0;
    }
    if(*x > bg_width - width) {
        *x = bg_width - width;
    }
    //if(*x > SCREEN_WIDTH - width) {
    //    *x = SCREEN_WIDTH - width;
    //}
    if(*y < 200) {
        *y = 200;
    }
    if(*y > bg_height - height) {
        *y = bg_height - height; 
    }
    //if(*y > SCREEN_HEIGHT - height) {
    //    *y = SCREEN_HEIGHT - height; 
    //}
}

void sprite_draw_health_bar(SDL_Renderer *renderer, Sprite *sprite, DArray *meta_info, int health_percentage) {
    
    int x_offset = 0;
    int y_offset = 0; 
    int width = 0;
    int height = 0;
    sprite_get_offset_wh_by_frame(sprite->animation, sprite->current_frame[sprite->animation], meta_info, &x_offset, &y_offset, &width, &height);

    //5 pixel height offset
    int offset = 15;
    int y_pos = sprite->y - offset;
    int x_pos = sprite->x;

    SDL_Rect pos;
	pos.x = x_pos;
	pos.y = y_pos;
    pos.w = 50;
    pos.h = 10;
    
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 0);
    
    int result = SDL_RenderDrawRect(renderer, &pos);
    if(result != 0) {
        printf("render rect failed: %s\n", SDL_GetError());
        exit(1);
    }

    //SDL_Surface *s;
    //s = SDL_CreateRGBSurface(0, pos.w, pos.h, 32, 0, 0, 0, 0);
    //SDL_FillRect(s, &pos, SDL_MapRGB(s->format, 255, 0, 0));
    
    
    SDL_Rect fill_rect;
    fill_rect.x = pos.x;
    fill_rect.y = pos.y;
    
    float ratio = (float)pos.w / (float)sprite->max_health;
    int w = ratio * sprite->health;
    
    //printf("ratio: %f health: %i width: %i\n", ratio, sprite->health, w);

    fill_rect.w = w;
    fill_rect.h = pos.h;
    
    SDL_RenderFillRect(renderer, &fill_rect);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
}


void sprite_draw_background(SDL_Renderer *renderer, SDL_Texture **backgrounds, int len, int *current_background, int player_x, int player_moving_right, int player_moving_left, int *player_scroll) { 
    
    SDL_Texture *background = NULL;
    int i = 0;
    for(i=0; i <len; i++) {
        if(i == *current_background) {
            //printf("[%p]\n", *(backgrounds+i));
            background = *(backgrounds + i);
            break;
        }
    }

    if(background != NULL) {
        int bg_slice = 0;
        if(player_x > SCROLL_WIDTH_RIGHT && player_moving_right) {
            *player_scroll = *player_scroll + 2;
        } else if (player_x < SCROLL_WIDTH_LEFT && player_moving_left) {
            *player_scroll = *player_scroll - 2;
        }
        
        SDL_Rect dst;
        //Query the texture to get its width and height to use
        SDL_QueryTexture(background, NULL, NULL, &dst.w, &dst.h);
        
        if(*player_scroll <= 0) {
            *player_scroll = 0;
        }
        

        if(*player_scroll >= dst.w - SCREEN_WIDTH) {
            *player_scroll = dst.w - SCREEN_WIDTH;
        }

        bg_slice = *player_scroll * -1;
        //printf("background\n");
        dst.x = bg_slice;
        dst.y = 0;
        //printf("bg_slice:%i destination width:%i height:%i \n", bg_slice, dst.w, dst.h);
        SDL_RenderCopy(renderer, background, NULL, &dst);
    }    
    /**        
            
        }

        SDL_Rect dst;
        dst.x = 0;
        dst.y = 0;
        //Query the texture to get its width and height to use
        SDL_QueryTexture(background, NULL, NULL, &dst.w, &dst.h);
        SDL_RenderCopy(renderer, background, NULL, &dst);
    
        SDL_Rect dsts;
        dsts.x = -700;
        dsts.y = 0;
        //Query the texture to get its width and height to use
        SDL_QueryTexture(background, NULL, NULL, &dsts.w, &dsts.h);
        SDL_RenderCopy(renderer, background, NULL, &dsts);
    }
    **/
    //number of backgrounds of any size even duplicates
    //current background keep track of
    //check to see if where at first or last
    //whether or not to keep scrolling if at end
    //current position is 400 but what is background at so current pos shows camera need a camera
    //camera is sdl rect it pans with current background
    //so camera shows current x at background
    //camera shows x position of all backgrounds strung together
    //but when do we scroll
    //when current position is within 150 pixels of end of screen only when moving in that direction
    //then we draw one pixel line more of background

    //decreases by 1 based on current positiion
    //SDL_Rect dst;
    //dst.x = -800;
    //dst.y = 0;
    //Query the texture to get its width and height to use
    //SDL_QueryTexture(background, NULL, NULL, &dst.w, &dst.h);
    //SDL_RenderCopy(renderer, background, NULL, &dst);
   
    //decreases by 1 based on current positiion
    //SDL_Rect dsts;
    //dsts.x = -700;
    //dsts.y = 0;
    //Query the texture to get its width and height to use
    //SDL_QueryTexture(background, NULL, NULL, &dsts.w, &dsts.h);
    //SDL_RenderCopy(renderer, background, NULL, &dsts);
    
    //what happens at boundry conditions 
    //when x = - 800
    //other x = 0
    
    
    //int current_pos = 0;
        //trigger shift at 10 pixel before max width
        //current_pos
        //how much further the next chunk of width is
        //if current pos is within range of first background then dont draw second background
        //draw background with x = current pos - width
        //original chunk of background with previous cutoff
        //draw background x = width - (current_pos - width)
        //when second background becomes first background repeat

}


int main(int argc, char** argv){
    printf("hello world\n");
    
    behaviors_init();
   
    log_file_open("log.txt");

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
    
    SDL_Texture *background1 = NULL;
    SDL_Texture *background2 = NULL;
    SDL_Texture *image = NULL;

    //background = sprite_load_image(renderer, "res/background_rcr.png");
    background1 = sprite_load_image(renderer, "res/background_rcr.png");
    background2 = sprite_load_image(renderer, "res/background_new.png");

    SDL_Texture* backgrounds[2];
    backgrounds[0] = background1;
    backgrounds[1] = background2;
    
    SDL_Rect bg;
    //Query the texture to get its width and height to use
    SDL_QueryTexture(background1, NULL, NULL, &bg.w, &bg.h);
    float bg_width = bg.w;
    float bg_height = bg.h;
    float world_width = bg_width / 10;
    float world_height = bg_height / 10;

    printf("bg_width: %f bg_height: %f world_width: %f world_height: %f\n", bg_width, bg_height, world_width, world_height);

    //printf("bg 1: [%p]\n", backgrounds[0]);
    //printf("bg 2: [%p]\n", backgrounds[1]);
	
    image = sprite_load_image(renderer, "res/output.png");
    //printf("after sprite load image\n");
        
    //load the meta info
    char* filename = "res/animation_meta_info.txt";
    DArray meta_info;
    darray_init(&meta_info);
    sprite_load_meta_file(filename, &meta_info);
    darray_print(&meta_info);

    float interval = 0.0f;
    float start = 0.0f;
    int quit = 0;
    
    float delay = 1000.0f / FPS;
    Sprite* player = sprite_create(PLAYER, WALK, 200, 300, delay, image);
    player->x_speed = 0;
    player->y_speed = 0;
    
    Sprite* enemy = sprite_create(ENEMY, WALK, 0, 300, delay, image);
    enemy->x_speed = 2;
    enemy->y_speed = 2;
    enemy->advance_frame = 1;
    //set white background
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    
    pixel_to_world(bg_width, bg_height, world_width, world_height,
                   enemy->x, enemy->y, &enemy->location->coords->x, &enemy->location->coords->y);
    
    int dest_x = 0;
    int dest_y = 0;
    
    //TODO:
    //animations for punch and basic kick
    //animation for getting hit
    //animations for walk left and run left
    //2 more ai behaviors
    //health bars above units and health stats
    //dying animation
    //dying logic
    //generate enemies off screen and have them walk on to screen
    //redo tornado kick and spinning back kick so stick figure is same size
    //figure out how to color stick figure without having to make new sprites on sheet...need to make figures white to set modulation
    //add rolling punch move
    //add game state which keeps track of game state
    //add buy moves screen in between levels
    //add a generator which generates guys for 100 levels

    while (!quit){
		start = SDL_GetTicks();
        
        quit = player_input(player, &meta_info);
        //printf("quit: %i\n", quit);  
       
        //log_msg(LOG_DEBUG, "after player input: current frame: %i\n", player->current_frame[HIT]);
        
        //pixel location to world location
        pixel_to_world(bg_width, bg_height, world_width, world_height, 
                       player->x, player->y, &player->location->coords->x, &player->location->coords->y);

        //world location to pixel location
        world_to_pixel(bg_width, bg_height, world_width, world_height,
                       enemy->location->coords->x, enemy->location->coords->y, &dest_x, &dest_y);
        
        //boundry check
        //boundry_check(SCREEN_WIDTH, SCREEN_HEIGHT, enemy, &meta_info, &dest_x, &dest_y);

        //printf("2nd before moveto = enemy x:%i enemy y:%i dest_x:%i dest_y:%i\n", enemy->x, enemy->y, dest_x, dest_y);
        
        //update enemy sprite by sprite's speed
        moveto_coordinates(enemy, dest_x, dest_y);
        
        //are we at the original world location in pixel coordinates?
        int arrived = within_coordinates(enemy, dest_x, dest_y);
        
        //printf("arrived: %i\n", arrived);
        if(arrived == 1) {
            //wander(enemy); 
            // we reached last behavior's destination so do new AI behavior
            //wander_attack(player, enemy, &meta_info, 4);
            wander_stall_attack(player, enemy, &meta_info, 3);
        }

        sprite_update_bounding_box(player, &meta_info);
        sprite_update_bounding_box(enemy, &meta_info);

        //check collision
        int collision = sprite_check_collision(player, enemy);
        
        //printf("collision: %i\n", collision);
        
        if(collision) {
            sprite_handle_collision(player, enemy, &meta_info);
        }
        
        //handle collision
        //if animation is an attack then check frame range that triggers a hit
        //check if attack animation triggers a hi or low hit
        //if sequence of attacks is within delta time then 4th hit triggers a knockdown
        //if attack would make health below 0 then it triggers a knockdown
        
        //handle opposite attack from opposing sprite
        //if is in attack animation then check frame range for a hit

        //update animation frame
        sprite_update(player, &meta_info, renderer);
        sprite_update(enemy, &meta_info, renderer);
        
        //Rendering
		SDL_RenderClear(renderer);
       
        //printf("player->x: %i\n", player->x);
        //printf("moving right: %i\n", player->moving_right);
        //printf("moving left: %i\n", player->moving_left);
        int current_background = 0;
        sprite_draw_background(renderer, backgrounds, 2, &current_background, player->x, player->moving_right, player->moving_left, &player->scroll);
        
        sprite_draw_health_bar(renderer, player, &meta_info, 100); 
        sprite_draw_health_bar(renderer, enemy, &meta_info, 100); 
        
        //draw sprite
        sprite_render_frame(SCREEN_WIDTH, SCREEN_HEIGHT, player, &meta_info, renderer, 0);
        sprite_render_frame(bg_width, bg_height, enemy, &meta_info, renderer, 0);

		//Update the screen
		SDL_RenderPresent(renderer);
         
        interval = SDL_GetTicks() - start;
        
        if(interval > 0) {
            //float fps = 1.0f / (interval / 1000.0f);
            //printf("%f fps\n", fps);
        }
	}
    
    //SDL_Delay(4000);

	//Destroy the various items
    sprite_destroy(player);
    sprite_destroy(enemy);
    darray_destroy(&meta_info);
	SDL_DestroyTexture(backgrounds[0]);
	SDL_DestroyTexture(backgrounds[1]);
	SDL_DestroyTexture(image);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
    
    printf("after destory text rend win\n");

	SDL_Quit();
    
    log_file_close();

    return 0;
}
