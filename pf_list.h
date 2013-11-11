#define PATH_SCORING_INIT 9999999

typedef struct coordinates {
    int x;
    int y;
} Coords;

typedef struct node {
    Coords *coords;
    struct node *parent;
    struct node *next;
    struct node *previous;
    int F;
    int G;
    int H;
} Node;

typedef struct pf_list {
    struct node *head;
    struct node *tail;
    int size;
} PF_List;

Coords* coords_create(int x, int y);
void coords_destroy(Coords *coords);
Node* node_create(int x, int y);
void node_destroy(Node *node);
Node* node_clone(Node *node);
void pf_list_init(PF_List *lst);
void pf_list_destroy(PF_List *lst);
int pf_list_size(PF_List *lst);
int pf_list_index(PF_List *lst, Node *node);
void pf_list_add(PF_List *lst, Node *node);
Node* pf_list_remove(PF_List *lst, Node *node);
void pf_list_move_list(PF_List *lst1, PF_List *lst2);
Node* pf_list_pop_lowest_fscore(PF_List *lst);
Node* pf_list_pop(PF_List *lst, int index);
void pf_list_print(PF_List *lst);












