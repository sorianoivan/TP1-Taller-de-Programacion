#include "server.h"

#define ERROR -1

void server_initialize(server_t* server){
    server->bind_skt = 0;
    server->peer_skt = 0;
}


int server_start(const char* port){

    server_t server;
    server_initialize(&server);

    if(server_bind(port, &server.bind_skt) != 0){
        return ERROR;
    }

    listen(server.bind_skt, 5);

    server.peer_skt = accept(server.bind_skt, NULL, NULL);

    char* msg = NULL;
    if(recv_message(msg, &server.peer_skt ) != 0){
        printf("error recieve");
        return -1;
    }

    return 0;

}
