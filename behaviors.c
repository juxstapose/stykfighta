#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include "main.h"
#include "pf_list.h"
#include "pathfinding.h"
#include "behaviors.h"


PF_List b_m_seek_lst;
PF_List b_m_invalid_lst;
Node *b_m_target;
int b_m_index = 0;
int b_m_count = 0;

void behaviors_init() {
    b_m_count = 0;
    srand(time(NULL));
    pf_list_init(&b_m_seek_lst);
    pf_list_init(&b_m_invalid_lst);
    b_m_target = node_create(-1,-1);
}

int behaviors_is_target_near(Node* origin, Node *target, int radius) {

    float x = target->coords->x - origin->coords->x;
    float y = target->coords->y - origin->coords->y;

    //calculate angle
    //1st quadrant
    float angle = 0.0f;
    //2nd quadrant 
    if(x < 0.0f && y < 0.0f) {
        angle = angle + ( M_PI / 2.0f);
    } 
    //3rd quadrant
    else if(x < 0.0f && y >= 0.0f) {
        angle = angle + M_PI;
    } 
    //4th quadrant
    else if(x >= 0.0f && y >= 0.0f) { 
        angle = angle + 3.0f * M_PI / 2.0f;
    }
    
    //printf("x:%f y:%f\n",x, y);
    float frac = y/x;
    if(frac < 0.0f) {
        frac = frac * -1;
    }

    //printf("y/x: %f\n", frac);
    //printf("atan(y/x) = %f\n", atan(frac));
    //printf("angle before atan: %f\n", angle);
    
    //find arc tan angle and add to base
    angle = angle + atan(frac);
    
    //calculate boundry coordinates to test against
    float x_test = origin->coords->x + radius * cos(angle);
    //flip signs because y goes down as it travels up (* -1)
    float y_test = origin->coords->y + radius * sin(angle) * -1;
    
    //printf("x_test y_test (%f,%f)\n", x_test, y_test);
    //printf("target coords (%i,%i)\n", target->coords->x, target->coords->y);
    //1st quadrant
    if( x >= 0.0f && y < 0.0f) {
        if(target->coords->x < x_test && target->coords->y > y_test) {
            return 1;
        }
    }
    //2nd quadrant
    if( x < 0.0f && y < 0.0f) {
        if(target->coords->x > x_test && target->coords->y > y_test) {
            return 1;
        }

    }
    //3rd quadrant
    if( x < 0.0f && y >= 0.0f) {
        if(target->coords->x > x_test && target->coords->y < y_test) {
            return 1;
        }
    }
    //4th quadrant
    if( x >= 0.0f && y >= 0.0f) {
        if(target->coords->x < x_test && target->coords->y < y_test) {
            return 1;
        }
    }
    return 0;
    /**
    int direction = 0;
    double direction_radians = (M_PI / 180) * (double)direction;
    float x_right = start->coords->x + radius * cos(direction_radians);
    float y_right = start->coords->y + radius * sin(direction_radians);
        
    direction = 180;
    direction_radians = (M_PI / 180) * (double)direction;
    float x_left = start->coords->x + radius * cos(direction_radians);
    float y_left = start->coords->y + radius * sin(direction_radians);
        
    direction = 90;
    direction_radians = (M_PI / 180) * (double)direction;
    float x_top = start->coords->x + radius * cos(direction_radians);
    float y_top = start->coords->y + radius * sin(direction_radians);

    direction = 270;
    direction_radians = (M_PI / 180) * (double)direction;
    float x_bottom = start->coords->x + radius * cos(direction_radians);
    float y_bottom = start->coords->y + radius * sin(direction_radians);

    printf("x left:%f y left:%f\n", x_left, y_left);
    printf("x right:%f y right:%f\n", x_right, y_right);
    printf("x top:%f y top:%f\n", x_top, y_top);
    printf("x bottom:%f y bottom:%f\n", x_bottom, y_bottom);
    
    int inside_x = 0;
    int inside_y = 0;
    if(target->coords->x <= x_right && target->coords->x >= x_left) {
        inside_x = 1;
    }
    if(target->coords->y <= y_top && target->coords->y >= y_bottom) {
        inside_y = 1;
    }
    if(inside_x && inside_y) {
        return 1;
    }
    **/
}

void behaviors_generate_location(Node* node, int *wander_direction) {
    int wander_offset = rand() % WANDER_MAX_OFFSET;    
    double pivot = ((double) rand() / (RAND_MAX));    
    if(pivot < .50f) {
        *wander_direction = *wander_direction + wander_offset;
    } else {
        *wander_direction = *wander_direction - wander_offset;
    }
    double wander_direction_radians = (M_PI / 180) * (double)*wander_direction;
    
    double xi = cos(wander_direction_radians);
    double yi = sin(wander_direction_radians);

    int x = node->coords->x;
    int y = node->coords->y;
    node->coords->x = round(x + xi);
    if(node->coords->x < 0) {
        node->coords->x = 0;
    }
    if(node->coords->x > WORLD_WIDTH) {
        node->coords->x = WORLD_WIDTH;
    }
    node->coords->y = round(y + yi);
    if(node->coords->y < 0) {
        node->coords->y = 0;
    }
    if(node->coords->y > WORLD_HEIGHT) {
        node->coords->y = WORLD_HEIGHT;
    }
    //printf("wander direction: %i\n", *wander_direction); 
    //printf("xi:%f yi:%f x:%i y:%i nx:%i ny:%i\n", xi, yi, x, y, node->coords->x, node->coords->y);
    
    //starting location srqt(2) units away is center of circle
    //circle has unit radius
    //initial x,y is determined by angle
    //subsequent x,y is determined by wander offset
}

void behaviors_seek(Node *start, Node *end, Node *result, int *count, int *length) {
    //move index one to next node in shortest path list
    b_m_index = 1;
    b_m_count++;
    //if end node has changed then change target to new end
    //find shortest path to new target
    if(b_m_target->coords->x != end->coords->x ||
       b_m_target->coords->y != end->coords->y) {
        pf_list_destroy(&b_m_seek_lst);
        pf_list_init(&b_m_seek_lst);
        //printf("find shortest path\n");
        find_shortest_path(start, end, &b_m_invalid_lst, &b_m_seek_lst);
        b_m_count = 1;
        *length = pf_list_size(&b_m_seek_lst); 
        b_m_target->coords->x = end->coords->x;
        b_m_target->coords->y = end->coords->y;
    }
    //fetch node at index
    //printf("before pop index:%i\n", b_m_index);
    //pf_list_print(&b_m_seek_lst);
    Node *r = pf_list_pop(&b_m_seek_lst, b_m_index);
    //printf("after pop [%p]\n", r);
    //pf_list_print(&b_m_seek_lst);
    //printf(" after pf list print\n");
    //printf("result: [%p]\n", result);
    //printf("r: [%p]\n", r);
    if(r != NULL) {
        result->coords->x = r->coords->x;
      //  printf(" after result coords x\n");
        result->coords->y = r->coords->y;
       // printf(" after result coords y\n");
        *count = b_m_count;
        //printf(" after count\n");
        //printf("RESULT: (%i,%i)\n", result->coords->x, result->coords->y);
    }
}

void behaviors_destroy() { 
    pf_list_destroy(&b_m_seek_lst);
    pf_list_destroy(&b_m_invalid_lst);
    node_destroy(b_m_target);
    b_m_count = 0;
}
/**
int main(int argc, char** argv) {
    
    Node *start = node_create(15, 10);
    Node *target = node_create(18, 8);
    int radius = 10;

    int r = behaviors_is_target_near(start, target, radius);
    printf("inside circle: %i\n", r);

    node_destroy(start);
    node_destroy(target);

    start = node_create(15, 10);
    target = node_create(10, 7);
    radius = 3;

    r = behaviors_is_target_near(start, target, radius);
    printf("inside circle: %i\n", r);

    node_destroy(start);
    node_destroy(target);
    
    start = node_create(15, 10);
    target = node_create(9, 15);
    radius = 3;

    r = behaviors_is_target_near(start, target, radius);
    printf("inside circle: %i\n", r);

    node_destroy(start);
    node_destroy(target);
    
    start = node_create(15, 10);
    target = node_create(21, 12);
    radius = 3;

    r = behaviors_is_target_near(start, target, radius);
    printf("inside circle: %i\n", r);

    node_destroy(start);
    node_destroy(target);
    
    return 0;
}
int main(int argc, char** argv){
    behaviors_init();
    
    Node *start = node_create(15, 10);
    Node *end = node_create(25, 20);
    //init target node off map so lst can be created initially
    b_m_target = node_create(-1,-1);

    Node *node = seek(start, end);
    printf("the node: (%i,%i)\n", node->coords->x, node->coords->y);
   
    node_destroy(node);

    int i = 0;
    for(i=0; i<20; i++) {
        //printf("i:%i\n",i);
        node = seek(start, end);
        if(node != NULL) {
            printf("the node: (%i,%i)\n", node->coords->x, node->coords->y);
            //pf_list_print(&b_m_seek_lst);
            node_destroy(node);
        }
    } 
    node_destroy(start);
    node_destroy(end);
    node_destroy(b_m_target);    

    start = node_create(15, 10);
    end = node_create(33, 28);
    b_m_target = node_create(-1,-1);
    node = seek(start, end);
    printf("the node: (%i,%i)\n", node->coords->x, node->coords->y);
    for(i=0; i<30; i++) {
        //printf("i:%i\n",i);
        node = seek(start, end);
        if(node != NULL) {
            printf("the node: (%i,%i)\n", node->coords->x, node->coords->y);
            //pf_list_print(&b_m_seek_lst);
            node_destroy(node);
        }
    }
    printf("start destroy\n");
    node_destroy(start);
    printf("start end\n");
    node_destroy(end);
    node_destroy(b_m_target);

    start = node_create(15, 10);
    end = node_create(33, 28); 
    b_m_target = node_create(-1,-1);
    node = seek(start, end);
    printf("the node: (%i,%i)\n", node->coords->x, node->coords->y);
    node_destroy(node);
    node_destroy(b_m_target);
    node_destroy(start);
    node_destroy(end);
    
    start = node_create(16, 11);
    end = node_create(30, 25); 
    b_m_target = node_create(-1,-1);
    node = seek(start, end);
    printf("the node: (%i,%i)\n", node->coords->x, node->coords->y);
    
    //pf_list_print(&b_m_seek_lst);
    node_destroy(node);
    node_destroy(b_m_target);
    node_destroy(start);
    node_destroy(end);
    
    behaviors_destroy();
    return 0;
}
int main(int argc, char** argv){
    printf("hello world\n");
    behaviors_init();
    int wander_direction = 90; 
    Node* start = node_create(15, 10);
    int i = 0;
    for(i=0; i< 100; i++) {
        wander(start, &wander_direction);
    }
}
**/


