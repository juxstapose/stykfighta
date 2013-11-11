
#define WANDER_MAX_OFFSET 45

void behaviors_init();
int behaviors_is_target_near(Node* origin, Node *target, int radius);
void behaviors_generate_location(Node *node, int *wander_direction);
void behaviors_seek(Node *start, Node *end, Node *result, int *count, int *length);
void behaviors_destroy(); 

