#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <sys/poll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include "log.h"
#include "network.h"

int _debug_level;

void log_init() {
    _debug_level = LOG_DEBUG;
}

void log_set_debug_level(int debug_level) {
    _debug_level = debug_level;
}

int log_connect(char* ip, int port, int retries, int timeout) {
    struct sockaddr_in daddr;
    int sock = 0;
    int y = 0;
    fd_set fdset;
    struct timeval tv;
    int retry=0;

    for(retry=0;retry<retries;retry++) {
        make_socket(ip, port, &sock, &daddr);
        if(sock == -1) {
            //fprintf(stderr, "received socket error: %s\n", strerror(errno));
            log_msg(sock, LOG_ERROR, "received socket error: %s\r\n", strerror(errno));
            exit(1);
        }

        if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &y, sizeof(int)) == -1) {
            //fprintf(stderr, "received socket error: %s\n", strerror(errno));
            log_msg(sock,LOG_ERROR, "received socket error: %s\r\n", strerror(errno));
            exit(1);
        }
        
        fcntl(sock, F_SETFL, O_NONBLOCK);
        tv.tv_sec = timeout;
        tv.tv_usec = 0;
        
        int r = connect(sock, (struct sockaddr *)&daddr, sizeof daddr);
        if(r == -1 && errno != EISCONN && errno != EINPROGRESS && errno != EALREADY) {
            //fprintf(stderr, "failed to connect to device %s retries: %i\n", strerror(errno), retries-retry);
            log_msg(sock, LOG_ERROR, "failed to connect to device %s retries: %i\r\n", strerror(errno), retries-retry);
        } else if(errno == EINPROGRESS || errno == EALREADY) {
            //fprintf(stdout, "connecting to socket num retrys: %i\n", retries-retry);
            log_msg(sock, LOG_INFO, "connecting to socket on %i and %s num retrys: %i\r\n", port, ip, retries-retry);
        } else {
            log_msg(sock, LOG_INFO, "connected to socket on port %i on %s\r\n", port, ip);
            break;
        }
        
        FD_ZERO(&fdset);
        FD_SET(sock, &fdset);
        if (select(sock + 1, NULL, &fdset, NULL, &tv) == 1) {
            int so_error;
            socklen_t slen = sizeof so_error;
            getsockopt(sock, SOL_SOCKET, SO_ERROR, &so_error, &slen);
            if (so_error == 0) {
                //fprintf(stdout, "connected to socket on port %i on %s\n", port, ip);
                log_msg(sock, LOG_INFO,  "connected to socket on port %i on %s\r\n", port, ip);
                break;
            } else {
                //fprintf(stderr, "connect to %i failed on ip %s %s\n", port, ip, strerror(so_error));
                log_msg(sock, LOG_ERROR, "connect to %i failed on ip %s %s\r\n", port, ip, strerror(so_error));
                close(sock);
                continue;
            }
        } else {
            //fprintf(stdout, "exceeded %i second timeout\n", timeout);
            log_msg(sock, LOG_INFO, "exceeded %i second timeout\r\n", timeout);
            close(sock);
            continue;
        }

    }//end for retries
    int opts;
    opts = fcntl(sock,F_GETFL);
    opts ^= O_NONBLOCK;
    fcntl(sock, F_SETFL, opts);
    if(retry == retries) {
        //fprintf(stderr, "failed to connect to log server %s on %i\n", ip, port);
        log_msg(sock, LOG_ERROR, "failed to connect to log server %s on %i\r\n", ip, port);
    }
    return sock;
}

int log_msg(int sock, int debug_level, const char* format, ...) {
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
    
    int ic = is_connected(sock);
    
    if(count > 0 && ic) {
        int total_bytes = 0;
        while(total_bytes < count) {
            int bytes = send(sock, dest, count, 0); 
            total_bytes+=bytes;
            if(bytes == -1 || bytes == 0) {
                fprintf(stderr, "received socket error %s\n", strerror(errno));
                break;
            }
        }
    }
    va_end(arglist);
    return count;
}
