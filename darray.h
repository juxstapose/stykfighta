enum { DARRAYSIZE = 5 };

typedef struct dynamic_array
{
    int max_size;
    int cur_size;
    int *items;
} DArray;

int darray_init(DArray *DAP);
void darray_destroy(DArray *DAP);
void darray_print(DArray *DAP);
void darray_add(DArray *DAP, int val);
int darray_length(DArray *DAP); 
int darray_full(DArray *DAP);
int darray_grow(DArray *DAP);

