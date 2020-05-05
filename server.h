#ifndef _SERVER_H
#define _SERVER_H

#include "common_socket.h"
#include "common_message_processor.h"
#include "common_printer.h"

typedef struct {
    int bind_skt;
    int peer_skt;
} server_t;

/* Constructor */
void server_initialize(server_t* self);

/* Comienza la ejecucion del server */
int server_start(server_t* self, const char* port);

/* Destructor */
void server_destroy(server_t* self);

#endif //_SERVER_H
