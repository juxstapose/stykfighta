#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "pf_list.h"
#include "pathfinding.h"

//enum { GRID_WIDTH = 100, GRID_HEIGHT = 100};

void find_adjacent_squares(Node *node_start, PF_List *current_list, PF_List *closed_list, PF_List *invalid_list) {
    Coords* start = node_start->coords;
    Node *adj1 = node_create(start->x, start->y+1);
    adj1->parent = node_start;
    //check to see if there is blocking terrain 
    int inval_index = pf_list_index(invalid_list, adj1);
    if(inval_index == -1) {
        //not a wall so check to see if on the final list
        int index = pf_list_index(closed_list, adj1);
        if(index == -1) {
            pf_list_add(current_list, adj1);
        }
    } else {
        node_destroy(adj1);
    }
    Node *adj2 = node_create(start->x, start->y-1);
    adj2->parent = node_start;
    inval_index = pf_list_index(invalid_list, adj2);
    if(inval_index == -1) {
        int index = pf_list_index(closed_list, adj2);
        if(index == -1) {
            pf_list_add(current_list, adj2);
        }
    } else {
        node_destroy(adj2);
    }
    
    Node *adj3 = node_create(start->x+1, start->y);
    adj3->parent = node_start;
    inval_index = pf_list_index(invalid_list, adj3);
    if(inval_index == -1) {
        int index = pf_list_index(closed_list, adj3);
        if(index == -1) {
            pf_list_add(current_list, adj3);
        }
    } else {
        node_destroy(adj3);
    }
    
    Node *adj4 = node_create(start->x-1, start->y);
    adj4->parent = node_start;
    inval_index = pf_list_index(invalid_list, adj4);
    if(inval_index == -1) {
        int index = pf_list_index(closed_list, adj4);
        if(index == -1) {
            pf_list_add(current_list, adj4);
        }
    } else {
        node_destroy(adj4);
    }
    
    Node *adj5 = node_create(start->x-1, start->y-1);
    adj5->parent = node_start;
    inval_index = pf_list_index(invalid_list, adj5);
    if(inval_index == -1) {
        int index = pf_list_index(closed_list, adj5);
        if(index == -1) {
            pf_list_add(current_list, adj5);
        }
    } else {
        node_destroy(adj5);
    }
    
    Node *adj6 = node_create(start->x+1, start->y+1);
    adj6->parent = node_start;
    inval_index = pf_list_index(invalid_list, adj6);
    if(inval_index == -1) {
        int index = pf_list_index(closed_list, adj6);
        if(index == -1) {
            pf_list_add(current_list, adj6);
        }
    } else {
        node_destroy(adj6);
    }
    
    Node *adj7 = node_create(start->x+1, start->y-1);
    adj7->parent = node_start;
    inval_index = pf_list_index(invalid_list, adj7);
    if(inval_index == -1) {
        int index = pf_list_index(closed_list, adj7);
        if(index == -1) {
            pf_list_add(current_list, adj7);
        }
    } else {
        node_destroy(adj7);
    }
    
    Node *adj8 = node_create(start->x-1, start->y+1);
    adj8->parent = node_start;
    inval_index = pf_list_index(invalid_list, adj8);
    if(inval_index == -1) {
        int index = pf_list_index(closed_list, adj8);
        if(index == -1) {
            pf_list_add(current_list, adj8);
        }
    } else {
        node_destroy(adj8);
    }
    
}

int is_diag(Node* node1, Node* node2) {
    if(node1->coords == NULL || node2->coords == NULL) { 
        printf("node coords is NULL\n");
        exit(EXIT_FAILURE);
    }
    if (node1->coords->x+1 == node2->coords->x && node1->coords->y+1 == node2->coords->y) {
        return 1;
    }
    if (node1->coords->x-1 == node2->coords->x && node1->coords->y-1 == node2->coords->y) {
        return 1;
    }
    if (node1->coords->x-1 == node2->coords->x && node1->coords->y+1 == node2->coords->y) {
        return 1;
    }
    if (node1->coords->x+1 == node2->coords->x && node1->coords->y-1 == node2->coords->y) {
        return 1;
    }
    return 0;
}

int calc_G(Node* parent_node, Node* node) {
    int result = 0;
    if(is_diag(parent_node, node)) {
        result = parent_node->G + 14;
    } else {
        result = parent_node->G + 10;
    }
    return result;
}

int calc_H(Node* node_start, Node* node_end) {
    if(node_start->coords == NULL || node_end->coords == NULL) { 
        printf("node coords is NULL\n");
        exit(EXIT_FAILURE);
    }
    int result = 0;
    int x_dist = abs(node_end->coords->x - node_start->coords->x);
    int y_dist = abs(node_end->coords->y - node_start->coords->y);
    result = (x_dist + y_dist) * 10;
    return result;
}

void calc_nodes_GHF(Node* selected_node, Node *node_end, PF_List *lst) {
    Node *curr = lst->tail->next;
    while(curr != lst->head) {
        curr->G = calc_G(curr->parent, curr);
        curr->H = calc_H(curr, node_end);
        curr->F = curr->G + curr->H;
        curr = curr->next;
    }
}

void check_g_score(Node* selected_node, PF_List *current_list, PF_List *open_list) {
    Node *curr = current_list->tail->next;
    //printf("%p\n", curr);
    while(curr != current_list->head) {
        //printf("before list index\n");
        int ng_index = pf_list_index(open_list, curr);
        //printf("ng_index: %i\n", ng_index);
        if(ng_index >= 0) {
            int g = calc_G(selected_node, curr);
            //printf("g: %i < G: %i\n",g,curr->G);
            if(g < curr->G) {
                curr->G = g;
                curr->F = g + curr->H;
                curr->parent = selected_node;
            }
        }
        curr = curr->next;
    }
}

void get_path(Node* node_start, PF_List *closed_list, PF_List *result) {
    //printf("print path\n");
    Node *curr = closed_list->tail->next;
    while(curr->coords->x != node_start->coords->x ||
          curr->coords->y != node_start->coords->y) {
        //printf("[%p] (%i,%i) F:%i ", curr, curr->coords->x, curr->coords->y, curr->F);
        Node* node = node_clone(curr);
        pf_list_add(result, node);
        curr = curr->parent;
    }
    //printf("[%p] (%i,%i) F:%i ", curr, curr->coords->x, curr->coords->y, curr->F);
    Node *node = node_clone(curr);
    pf_list_add(result, node);
}

void find_shortest_path(Node *node_start, Node *node_end, PF_List *invalid_list, PF_List *result) {
    
    //printf("initialize lists\n");
    PF_List open_list;
    PF_List closed_list;
    PF_List current_list;
    pf_list_init(&open_list);
    pf_list_init(&closed_list);
    pf_list_init(&current_list);

    Node *start_copy = node_clone(node_start);
    
    //printf("create nodes and add start to open list\n");
    start_copy->F = 9999999;
    pf_list_add(&open_list, start_copy);
    
    int i = 0;

    while(1) {
        //printf("pop lowest fscore and add to closed list\n");
        //pf_list_print(&open_list);
        //find lowest fscore for the current selected square
        
        Node* selected_node = pf_list_pop_lowest_fscore(&open_list);
        pf_list_add(&closed_list, selected_node);
        
        //printf("find adjacent squares\n");
        //find squares next to selected node
        find_adjacent_squares(selected_node, &current_list, &closed_list, invalid_list);
        //printf("calc nodes GHF\n");
        calc_nodes_GHF(selected_node, node_end, &current_list);
        
        //put nodes from current list to open list
        //clear current list for next go around
        
        //printf("current list %i\n", i);
        //pf_list_print(&current_list);
        //printf("open list %i\n", i);
        //pf_list_print(&open_list);
        
        //iterate through current list to see if adajcent square is on open list
        //if the adjacent square is on the open list...calc g score with selected node and the current adjacent square
        //that g score is less than the node's current g score
        //make the selected node the parent of that node and set its gscore to the new one
        //recalculate f score
        
        //printf("check gscore\n");
        check_g_score(selected_node, &current_list, &open_list);

        //printf("clear current list and move items to open list\n");
        pf_list_move_list(&current_list, &open_list);
        
        //check to see if the end node is on the closed list
        //if it is then we are done
        int index = pf_list_index(&closed_list, node_end);
        //printf("find index of end node in closed list: %i\n", index);

        //printf("current list %i\n", i);
        //pf_list_print(&current_list);
        //printf("open list %i\n", i);
        //pf_list_print(&open_list);
        
        //printf("closed list %i\n", i);
        //pf_list_print(&closed_list);
        
        i++;
        //if(i == 11) {
        //    break;
        //}
        if(index >= 0) {
            break;
        }
    }
    
    get_path(node_start, &closed_list, result);
    
    //pf_list_print(result);

    //printf("open list destroy\n");
    pf_list_destroy(&open_list);
    //printf("closed list destroy\n");
    pf_list_destroy(&closed_list);
    //printf("current list destroy\n");
    pf_list_destroy(&current_list);
}
/**
int main(int argc, char** argv){
    
    printf("hello world\n");
    
    //int grid[GRID_WIDTH][GRID_HEIGHT];
    
    Node *start = node_create(1,2);
    Node *end = node_create(5,2);
    
    PF_List invalid_list;
    pf_list_init(&invalid_list);
    
    Node *wall1 = node_create(3,1);
    Node *wall2 = node_create(3,2);
    Node *wall3 = node_create(3,3);

    pf_list_add(&invalid_list, wall1);
    pf_list_add(&invalid_list, wall2);
    pf_list_add(&invalid_list, wall3);

    PF_List result;
    pf_list_init(&result);

    //printf("find shortest path\n");
    find_shortest_path(start, end, &invalid_list, &result);
    
    //printf("result:\n");
    pf_list_print(&result);

    pf_list_destroy(&invalid_list);
    pf_list_destroy(&result);
}
**/
