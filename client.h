#ifndef _CLIENT_H
#define _CLIENT_H

#include "common_socket.h"
#include "common_message_processor.h"
#include "common_printer.h"

typedef struct {
    int client_skt;
} client_t;

/* Constructor */
void client_initialize(client_t* self);

/* Comienza la ejecucion del cliente */
int client_start(client_t* self, const char* host, const char* port,
        const char* filename);

/* Destructor */
void client_destroy(client_t* self);

#endif //_CLIENT_H
