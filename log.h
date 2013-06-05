#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LOG_DEBUG 0
#define LOG_INFO 1
#define LOG_ERROR 2
#define LOG_OFF 3

void log_init();
void log_set_debug_level(int debug_level);
int log_connect(char* ip, int port, int retries, int timeout);
int log_msg(int sock, int debug_level, const char* format, ...);
