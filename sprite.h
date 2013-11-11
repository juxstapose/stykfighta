
#define MAX_ANIMATIONS 10

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
    int current_frame[MAX_ANIMATIONS];
    int key_state;
    int right_key;
    int left_key;
    int up_key;
    int down_key;
    int wander_direction;
    int state;
    int wander_start;
    int stall_start;
    int wander_duration;
    int stall_duration;
    SDL_Rect bounding_box;
    Node *location;
    int seek_count;
    int seek_length;
    int flee_loc_x;
    int flee_loc_y;
    int animation_count[MAX_ANIMATIONS];
    int last_animation_count[MAX_ANIMATIONS];
    int direction;
} Sprite;

int within_coordinates(Sprite *sprite, int dest_x, int dest_y);
void moveto_coordinates(Sprite *sprite, int dest_x, int dest_y);
void world_to_pixel(int x, int y, int *dest_x, int *dest_y);
void pixel_to_world(int x, int y, int *dest_x, int *dest_y);
int is_within(int cmp1, int cmp2, int fudge);
SDL_Texture* sprite_load_image(SDL_Renderer *renderer, char* filename);
int sprite_get_frames(int animation, DArray *meta_info);
Sprite* sprite_create(int type, int animation, int x, int y, float delay, SDL_Texture *texture);
void sprite_handle_collision(Sprite *sprite1, Sprite *sprite2, DArray *meta_info);
void sprite_destroy(Sprite *sprite);
void sprite_update(Sprite* sprite, DArray *meta_info, SDL_Renderer *rend);
void sprite_render_frame(Sprite *sprite, DArray *meta_info, SDL_Renderer *rend, int draw_bounding_box);
void sprite_load_meta_file(char* filename, DArray *meta_info);
void sprite_get_offset_wh_by_frame(int animation, int frame, DArray* meta_info, int *x_offset, int *y_offset, int* width, int* height);
