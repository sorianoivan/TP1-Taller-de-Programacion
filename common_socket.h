#ifndef _SOCKET_H
#define _SOCKET_H

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

int set_up_connection(const char* host, const char* port, int* skt, const char* mode);

int try_recv(void* buff, u_int32_t buff_len, const int skt);

int try_send(const void* msg, int msg_len, const int skt);

#endif //_SOCKET_H
