#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LOG_DEBUG 0
#define LOG_INFO 1
#define LOG_ERROR 2
#define LOG_OFF 3

void log_init();
void log_set_debug_level(int debug_level);
int log_msg(int debug_level, const char* format, ...);
void log_file_open(const char* filename);
void log_file_close(); 
