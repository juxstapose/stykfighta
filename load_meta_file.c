#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "darray.h"

enum { LINE_BUFF_SIZE = 128 };

void sprite_load_meta_file(char* filename, DArray *meta_info) {
    FILE *file = fopen ( filename, "r" );
    char line[LINE_BUFF_SIZE];
    char* pch;
    memset(line, '\0', LINE_BUFF_SIZE);
    darray_init(meta_info);
    if ( file != NULL ) {
        while(fgets(line, sizeof line, file) != NULL) {
            int val = 0;
            fputs (line, stdout);
            pch = strtok (line," ,.-");
            val = atoi(pch);
            darray_add(meta_info, val);
            while(pch != NULL) {
                printf ("%i\n",val);
                pch = strtok (NULL, " ,.-");
                if(pch != NULL) {
                    val = atoi(pch);
                    darray_add(meta_info, val);
                }
            } 
        }
        fclose(file);
   }
   else {
      perror(filename);
   } 
}

int main(int argc, char** argv){
    char* filename = "animation_meta_info.txt";
    DArray meta_info;
    darray_init(&meta_info);
    sprite_load_meta_file(filename, &meta_info);
    printf("meta info\n");
    darray_print(&meta_info);
    darray_destroy(&meta_info);
}

