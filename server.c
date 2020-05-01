#include "server.h"

#define ERROR -1

void server_initialize(server_t* server){
    server->bind_skt = 0;
    server->peer_skt = 0;
}

char* recv_message(char* msg, int* skt, int* recieved){
    int bytes_read = 0;
    int id = 0;
    u_int32_t body_len = 0;
    u_int32_t header_len = 0;

    msg = malloc(16); //capaz le tengo q sumar 1
    memset(msg, 0, 16);

    char* msg2 = msg + 16;

    *recieved = try_recv(msg, 16, skt);        //lee la primera linea del header
    bytes_read += 4; //ignoro los primeros 4 bytes

    body_len = *( (uint32_t *) (msg + bytes_read)); //largo del body
    bytes_read += 4;

    id = *(msg + bytes_read); //id
    bytes_read += 4;
    id += 1;
    id -= 1;

    header_len = *( (uint32_t *) (msg + bytes_read) );//largo del body

    int padding_firma = (int)(8 - (header_len) % 8) % 8;

    msg = realloc(msg,header_len + body_len + padding_firma);
    memset(msg, 0, header_len + body_len + padding_firma);

    *recieved = try_recv(msg, header_len + body_len + padding_firma, skt);

    return msg;
}

int server_start(const char* port){

    server_t server;
    server_initialize(&server);

    if(server_bind(port, &server.bind_skt) != 0){
        return ERROR;
    }
    listen(server.bind_skt, 5);
    server.peer_skt = accept(server.bind_skt, NULL, NULL);

    int recieved = 0;
    char* msg = NULL;
    do {
        msg = recv_message(msg, &server.peer_skt, &recieved);

        for (int i = 0; i < recieved; ++i) {
            printf("%c", *(msg + i));
        }
        if(recieved == -1){
            return -1;
        }
        if(msg != NULL){
          free(msg);
        }

        if(try_send("OK", 3, &server.peer_skt) != 0){
            printf("error send OK\n");
            return -1;
        }
    }while(recieved != 0);


    shutdown(server.bind_skt, SHUT_RDWR);
    shutdown(server.peer_skt, SHUT_RDWR);
    close(server.bind_skt);
    close(server.peer_skt);

    return 0;
}
