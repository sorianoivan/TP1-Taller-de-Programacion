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
    int len = 0;
    if(try_recv(&len, 4, &server.peer_skt ) != 0){   //recibe el largo del mensaje
        printf("error recieve");
        return -1;
    }

    msg = malloc(len + 1);
    memset(msg, 0, len + 1);

    if(try_recv(msg, len, &server.peer_skt) != 0){
        printf("error recieve");
        return -1;
    }
    printf("%s", msg);
    free(msg);

    shutdown(server.bind_skt, SHUT_RDWR);
    shutdown(server.peer_skt, SHUT_RDWR);
    close(server.bind_skt);
    close(server.peer_skt);

    return 0;
}
