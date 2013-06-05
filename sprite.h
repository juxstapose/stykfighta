enum { TORNADO_KICK = 0, SPINNING_BACK_KICK = 1, WALK = 2, RUN = 3, LINE_BUFF_SIZE = 128, SCREEN_WIDTH=640, SCREEN_HEIGHT=480, PLAYER=0, ENEMY=1, FPS=30};

typedef struct sprite_sprite {
    int type;
    int animation;
    int x;
    int y;
    SDL_Texture *texture;
    float delay;
    int x_speed;
    int y_speed;
    int advance_frame;
    float last_update;
    int current_frame;
    int key_state;
} Sprite;

SDL_Texture* sprite_load_image(SDL_Renderer *renderer, char* filename);
int sprite_get_frames(int animation, DArray *meta_info);
Sprite* sprite_create(int type, int animation, int x, int y, float delay, SDL_Texture *texture);
void sprite_destroy(Sprite *sprite);
void sprite_update(Sprite* sprite, DArray *meta_info, SDL_Renderer *rend);
void sprite_render_frame(int animation, int current_frame, int x, int y, DArray *meta_info, SDL_Texture *tex, SDL_Renderer *rend);
void sprite_load_meta_file(char* filename, DArray *meta_info);
void sprite_get_offset_wh_by_frame(int animation, int frame, DArray* meta_info, int *x_offset, int *y_offset, int* width, int* height);

