#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <sys/poll.h>
#include <sys/types.h>
#include "log.h"

int _debug_level;
FILE *_lfp;

void log_init() {
    _debug_level = LOG_DEBUG;
}

void log_set_debug_level(int debug_level) {
    _debug_level = debug_level;
}

void log_file_open(const char* filename) {
    _lfp = fopen(filename, "w");
    if(_lfp == NULL) {
        fclose(_lfp);
    }
}

void log_file_close() { 
    fclose(_lfp);
}

int log_msg(int debug_level, const char* format, ...) {
    //printf("log msg %i < %i\n", debug_level, _debug_level);
    if(debug_level < _debug_level) {
        return 0;
    }
    
    char dest[1024 * 16];
    int count = 0;
    va_list arglist;
    va_start(arglist, format);

    if(debug_level == LOG_DEBUG) {
        strcpy(dest, "DEBUG: ");
        count = vsnprintf(&dest[7], 1024*16-7, format, arglist);
        count+=7;
    }
    if(debug_level == LOG_INFO) {
        strcpy(dest, "INFO: ");
        count = vsnprintf(&dest[6], 1024*16-6, format, arglist);
        count+=6;
    }
    if(debug_level == LOG_ERROR) {
        strcpy(dest, "ERROR: ");
        count = vsnprintf(&dest[7], 1024*16-7, format, arglist);
        count+=7;
    }
    
    printf("%s", dest);
    if(_lfp != NULL) {
        fwrite(dest, 1, strlen(dest), _lfp);
    }

    va_end(arglist);
    return count;
}
/**
int main(int argc, char** argv){
    log_init();
    int count = 10;
    int bytes = log_msg(LOG_DEBUG, "hello this is a debug test with the number %i", count);
}
**/
