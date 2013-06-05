#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

enum { GRID_WIDTH = 100, GRID_HEIGHT = 100};

void find_shortest_path(Coords *start, Coords *end) {
    
    PF_List open_list;
    PF_List closed_list;
    pf_list_init(&open_list);
    pf_list_init(&closed_list);
    
    find_adjacent_squares(start, &open_list);
    
    pf_list_destroy(&open_list);
    pf_list_destroy(&closed_list);
}

int main(int argc, char** argv){
    
    printf("hello world\n");
    
    int grid[GRID_WIDTH][GRID_HEIGHT];
    
    Coords *start = coords_create(0,0);
    start->x = 15;
    start->y = 20;

    Coords *end = coords_create(0,0);
    end->x = 75;
    end->y = 65;
    
    PF_List closed_list;
    pf_list_init(closed_list);

    find_shortest_path(start, end, &closed_list);
    
    pf_list_print(&closed_list);

    coords_destroy(start);
    coords_destroy(end);
    pf_list_destroy(&closed_list);

}

