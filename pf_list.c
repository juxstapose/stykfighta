#include <stdio.h>
#include <stdlib.h>

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


Coords* coords_create(int x, int y) {
    Coords *coords = (Coords*)malloc(sizeof(Coords));
    if(coords == NULL) {
        printf("failed to alloc memory for coords\n");
        exit(EXIT_FAILURE);
    }
    coords->x = x;
    coords->y = y;
    return coords;
}

void coords_destroy(Coords *coords) {
    free(coords);
}

Node* node_create(int x, int y) {
    Coords *coords = (Coords*)malloc(sizeof(Coords));
    if(coords == NULL) {
        printf("failed to alloc memory for coords\n");
        exit(EXIT_FAILURE);
    }
    coords->x = x;
    coords->y = y;
    Node *node = (Node*)malloc(sizeof(Node));
    if(node == NULL) {
        printf("failed to alloc memory for node\n");
        exit(EXIT_FAILURE);
    }
    node->next = NULL;
    node->previous = NULL;
    node->parent = NULL;
    node->F = 0;
    node->G = 0;
    node->H = 0;
    node->coords = coords;
    return node;
}

void node_destroy(Node *node) {
    if(node->coords != NULL) {
        free(node->coords);
    }
    if(node != NULL) {
        free(node);
    }
}

void pf_list_init(PF_List *lst) {
    if(lst->head == NULL) {
        Node *h = node_create(0,0);
        free(h->coords);
        h->coords = NULL;
        lst->head = h;
    }
    if(lst->tail == NULL) {
        Node *t = node_create(0,0);
        free(t->coords);
        t->coords = NULL;
        lst->tail = t;
    }
    lst->head->next = lst->tail;
    lst->head->previous = NULL;
    lst->tail->next = NULL;
    lst->tail->previous = lst->head;
    lst->size = 0;
}

void pf_list_destroy(PF_List *lst) {
    if(lst->head != NULL) {
        node_destroy(lst->head);
    }
    if(lst->tail != NULL) {
        node_destroy(lst->tail);
    }
}

void pf_list_add(PF_List *lst, Node *node) {
    node->previous = lst->tail->previous;
    node->previous->next = node;
    lst->tail->previous = node;
    node->next = lst->tail;
    lst->tail->next = NULL;
    lst->size++;
}

Node* pf_list_remove(PF_List *lst, Node *node) {
    Node *curr = lst->head;
    while(curr != NULL) {
        if(curr->coords != NULL &&
           curr->coords->x == node->coords->x &&
           curr->coords->y == node->coords->y) {
            //unlink then return ptr
            Node* p = curr->previous;
            Node* n = curr->next;
            p->next = n;
            n->previous = p;
            lst->size--;
            return curr;
        }
        curr = curr->next;
    }
}

void pf_list_print(PF_List *lst){
    Node *curr = lst->head;
    while(curr != NULL) {
        if(curr->coords != NULL) {
            printf("(%i,%i) ", curr->coords->x, curr->coords->y);
        }
        curr = curr->next;
    }
    printf("\n");
}

int main(int argc, char** argv){
    printf("hello world\n");
    PF_List open_list;
    pf_list_init(&open_list);
    Node *ele = node_create(20, 25);
    Node *ele2 = node_create(120, 5);
    Node *ele3 = node_create(10, 40);
    Node *ele4 = node_create(30, 7);
    pf_list_add(&open_list, ele);
    pf_list_add(&open_list, ele2);
    pf_list_add(&open_list, ele3);
    pf_list_add(&open_list, ele4);
    pf_list_print(&open_list);
    
    pf_list_remove(&open_list, ele);
    pf_list_print(&open_list);
    
    pf_list_remove(&open_list, ele2);
    pf_list_print(&open_list);
    
    pf_list_remove(&open_list, ele4);
    pf_list_print(&open_list);
    
    pf_list_remove(&open_list, ele3);
    pf_list_print(&open_list);

    node_destroy(ele);
    node_destroy(ele2);
    node_destroy(ele3);
    node_destroy(ele4);
    pf_list_destroy(&open_list);
    

}





