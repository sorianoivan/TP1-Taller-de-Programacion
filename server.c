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


    int socket_connected = 1;
    while(socket_connected != 0) {
        char* msg = NULL;
        int len = 0;
        socket_connected = try_recv(&len, 4, &server.peer_skt);   //recibe el largo del mensaje
        if(socket_connected == -1){                                       //esto lo tengo q cambiar para q lea el largo del protocolo
            printf("error recieve"); //IMPRIMIR ERRORES EN STDERR
            return -1;
        }
        msg = malloc(len + 1);
        memset(msg, 0, len + 1);

        socket_connected = try_recv(msg, len, &server.peer_skt);        //recibe mensaje
        if(socket_connected == -1) {
            printf("error recieve");
            return -1;
        }
        for (int i = 0; i < socket_connected ; ++i) {
            printf("%c", *(msg + i));
        }
        free(msg);
    }

    shutdown(server.bind_skt, SHUT_RDWR);
    shutdown(server.peer_skt, SHUT_RDWR);
    close(server.bind_skt);
    close(server.peer_skt);

    return 0;
}
