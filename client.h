#ifndef _CLIENT_H
#define _CLIENT_H

#include "common_socket.h"

typedef struct {
    int client_skt;
} client_t;

void client_initialize(client_t* client);
int client_start(const char* host, const char* port, const char* filename);


#endif //_CLIENT_H
