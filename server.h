#ifndef _SERVER_H
#define _SERVER_H

#include "common_socket.h"
#include "common_message.h"

typedef struct {
    int bind_skt;
    int peer_skt;
} server_t;

int server_start(const char* port);

#endif //_SERVER_H
