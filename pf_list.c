#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "pf_list.h"

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

Node* node_clone(Node* orig) {
    Coords *coords = (Coords*)malloc(sizeof(Coords));
    if(coords == NULL) {
        printf("failed to alloc memory for coords\n");
        exit(EXIT_FAILURE);
    }
    Node *node = (Node*)malloc(sizeof(Node));
    if(node == NULL) {
        printf("failed to alloc memory for node\n");
        exit(EXIT_FAILURE);
    }
    node->next = orig->next;
    node->previous = orig->previous;
    node->parent = orig->parent;
    node->F = orig->F;
    node->G = orig->G;
    node->H = orig->H;
    coords->x = orig->coords->x;
    coords->y = orig->coords->y;
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
    Node *h = node_create(0,0);
    free(h->coords);
    h->coords = NULL;
    lst->head = h;
    
    Node *t = node_create(0,0);
    free(t->coords);
    t->coords = NULL;
    lst->tail = t;
    
    lst->head->next = NULL;
    lst->head->previous = NULL;
    lst->tail->next = NULL;
    lst->tail->previous = NULL;
    lst->size = 0;
}

int pf_list_size(PF_List *lst) {
    return lst->size;
}   

void pf_list_destroy(PF_List *lst) {
    int i = 0;
    int size = lst->size;
    for(i=0; i<size; i++) {
        Node *curr = pf_list_pop(lst, 0);
        node_destroy(curr);
    }
    if(lst->head != NULL) {
        node_destroy(lst->head);
    }
    if(lst->tail != NULL) {
        node_destroy(lst->tail);
    }
}

int pf_list_index(PF_List *lst, Node *node) {
    Node *curr = lst->tail->next;
    int i = 0;
    while(curr != NULL && curr != lst->head) {
        //printf("%p\n", curr);
        if(curr->coords != NULL &&
           curr->coords->x == node->coords->x &&
           curr->coords->y == node->coords->y) {
            return i;
        }
        i++;
        curr = curr->next;
    }
    return -1;
}

void pf_list_add(PF_List *lst, Node *node) {
    node->previous = lst->tail;
    if(lst->tail->next == NULL) {
        //if the tail's next points to null then we have an empty list
        assert(lst->size == 0);
        node->next = lst->head;
    } else {
        //point the nodes next pointer to what the tail's next is pointing at before we change it
        node->next = lst->tail->next;
        lst->tail->next->previous = node;
    }
    lst->tail->next = node;
    if(lst->head->previous == NULL) {
        //if the head's previous points to null then we have an empty list
        assert(lst->size == 0);
        lst->head->previous = node;
    }
    lst->head->next = NULL;
    lst->tail->previous = NULL;
    lst->size++;
}

Node* pf_list_remove(PF_List *lst, Node *node) {
    Node *curr = lst->tail->next;
    while(curr != lst->head) {
        //printf("[%p] (%i,%i) == (%i,%i)\n", curr, curr->coords->x, curr->coords->y, node->coords->x, node->coords->y);
        if(curr->coords != NULL &&
           curr->coords->x == node->coords->x &&
           curr->coords->y == node->coords->y) {
            //unlink then return ptr
            Node* p = curr->previous;
            Node* n = curr->next;
            //printf("current %p\n", curr);
            //printf("previous %p\n", p);
            //printf("next %p\n", n);
            p->next = n;
            n->previous = p;
            lst->size--;
            return curr;
        }
        curr = curr->next;
    }
    return NULL;
}


void pf_list_move_list(PF_List *lst1, PF_List *lst2) {
    Node *curr = lst1->tail->next;
    while(curr != lst1->head) {
        pf_list_remove(lst1, curr);
        //printf("current list\n");
        //pf_list_print(lst1);
        
        //clone node
        //move next pointer
        //add clone to lst2
        //destroy node in memory
        Node* copy = node_clone(curr);
        node_destroy(curr);
        curr = NULL;
        curr = copy->next;
        //check to see if there is a duplicate
        //if there is a duplicate then choose the node with the lowest fscore
        Node *existing = pf_list_remove(lst2, copy);
        if(existing != NULL) {
            if(copy->F < existing->F) {
                pf_list_add(lst2, copy);
            } else {
                pf_list_add(lst2, existing);
            }
        } else {
            pf_list_add(lst2, copy);
        }
        
        //printf("open list\n");
        //pf_list_print(lst2);

        //TODO reference to tail points to lst2 after added 
    }
}

Node* pf_list_pop_lowest_fscore(PF_List *lst) {
    Node *curr = lst->tail->next;
    int f_score_min = 9999999;
    Node * result = NULL;
    while(curr != NULL && curr != lst->head) {
        //printf("[%p] (%i,%i) F:%i previous: [%p]\n", curr, curr->coords->x, curr->coords->y, curr->F, curr->previous);
        if(f_score_min >= curr->F) {
            f_score_min = curr->F;
            result = curr;
        }
        curr = curr->next;
    }
    //printf("removed lowest [%p] F:%i (%i,%i) \n", result, result->F, result->coords->x, result->coords->y);
    pf_list_remove(lst, result);
    return result;
}

Node* pf_list_pop(PF_List *lst, int index) {
    int i = 0;
    Node *curr = lst->tail->next;
    while(curr!= NULL && curr != lst->head) {
        //printf("pf_list_pop:%i == %i\n", i, index);
        if(index == i) {
            Node* p = curr->previous;
            Node* n = curr->next;
            p->next = n;
            n->previous = p;
            lst->size--;
          //  printf("return curr\n");
            return curr;
        }
        i++;
        curr = curr->next;
    }
    return NULL;
}

void pf_list_print(PF_List *lst){
    Node *curr = lst->tail->next;
    if(curr != NULL) {
        while(curr != lst->head) {
            if(curr->coords != NULL) {
                printf("[%p] (%i,%i) F:%i ", curr, curr->coords->x, curr->coords->y, curr->F);
            }
            curr = curr->next;
        }
    }
    printf("\nlist size: %i\n", lst->size);
}

/**
int main(int argc, char** argv){
    printf("hello world\n");
    //PF_List open_list;
    //pf_list_init(&open_list);
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
    
    pf_list_remove(&open_list, ele4);
    pf_list_print(&open_list);
    
    pf_list_remove(&open_list, ele3);
    pf_list_print(&open_list);
    node_destroy(ele);
    node_destroy(ele2);
    node_destroy(ele3);
    node_destroy(ele4);
    //pf_list_destroy(&open_list);
}
**/
