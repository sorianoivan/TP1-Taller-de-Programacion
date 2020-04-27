#ifndef _SOCKET_H
#define _SOCKET_H

#define _POSIX_C_SOURCE 200112L

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

/*typedef struct{
    int skt_fd; //socket file descriptor
} socket_t;*/

int client_connect(const char* host, const char* port, int* skt);

int server_bind(const char* port, int* skt);

//int send_message(const char* msg, const int* skt);

int try_recv(void* buff, int buff_len, const int* skt);

int try_send(const void* msg, int msg_len, const int* skt);

#endif //_SOCKET_H
