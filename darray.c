#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "darray.h"

int darray_init(DArray *DAP) {
    DAP->items=(int *)malloc(sizeof(int)*DARRAYSIZE);
    if (DAP->items) {
        DAP->max_size = DARRAYSIZE;
        DAP->cur_size = 0;
        return 0;
    }
    else {
        printf("DArray malloc failed!\n");
        return 1;
    }
}

void darray_print(DArray *DAP)
{
    printf("Max = %d; Cur = %d\n", DAP->max_size, DAP->cur_size);
    int i = 0;
    for(i = 0; i < DAP->cur_size; i++) {
        printf("%d\t", DAP->items[i]);
    }
    printf("\n");
}

void darray_add(DArray *DAP, int val)
{
    if (!darray_full(DAP)) {
        DAP->items[DAP->cur_size++] = val;
    }
    else if (!darray_grow(DAP)) {
        DAP->items[DAP->cur_size++] = val;
    }
    else {
        printf("darray add failure\n");
        exit(EXIT_FAILURE);
    }
}

int darray_full(DArray *DAP)
{
    assert(DAP->cur_size >= 0 && DAP->max_size >= 0);
    assert(DAP->cur_size <= DAP->max_size);
    if (DAP->cur_size == DAP->max_size) {
        return 1;
    }
    else {
        return 0;
    }
}

//grows the DArray to double its original size
int darray_grow(DArray *DAP) {
    int *temp=(int *)realloc(DAP->items, sizeof(*temp) * DAP->max_size * 2);
    if (!temp) {
        printf("Realloc failed!\n");
        return 1;
    }
    else {
        DAP->items = temp;
        DAP->max_size *= 2;
        printf("Darray doubled and current contents are:\n");
        darray_print(DAP);
        return 0;
    }
}

int darray_length(DArray *DAP) { 
    return DAP->cur_size;
}

void darray_destroy(DArray *DAP){
    free(DAP->items);
    DAP->items = 0;
    DAP->max_size = 0;
    DAP->cur_size = 0;
}
/**
int main(int argc, char** argv){
    printf("testing darray\n");
    DArray da;
    int i = 0;
    if (!darray_init(&da)) {
        for (i = 0; i < 30; i++) {
            darray_add(&da, i);
        }
        darray_print(&da);
    }
    else {
        exit(EXIT_FAILURE);
    }
    darray_destroy(&da);
}
**/
