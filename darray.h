enum { DARRAYSIZE = 5 };

typedef struct dynamic_array
{
    int max_size;
    int cur_size;
    int *items;
} DArray;

extern int darray_init(DArray *DAP);
extern void darray_destroy(DArray *DAP);
extern void darray_print(DArray *DAP);
extern void darray_add(DArray *DAP, int val);
extern int darray_length(DArray *DAP); 
static int darray_full(DArray *DAP);
static int darray_grow(DArray *DAP);

