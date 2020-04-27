#include "client.h"

#define ERROR -1

void client_initialize(client_t* client) {
    client->client_skt = 0;
}

int send_message(const char* msg, int* skt){
    int len = (int)strlen(msg);
    if(try_send(&len, sizeof(int), skt) != 0){
        printf("error send");
        return -1;
    }

    if(try_send(msg, len, skt) != 0){
        printf("error send");
        return -1;
    }
    return 0;
}

int client_start(const char* host, const char* port, const char* filename) {
    client_t client;
    client_initialize(&client);

    if(client_connect(host, port, &client.client_skt) != 0) {     //ver si puedo sacar el != 0
        printf("Error conectando cliente");
        return ERROR;
    }



    int s = send_message(filename, &client.client_skt);

    shutdown(client.client_skt, SHUT_RDWR);
    close(client.client_skt);

    return s;
}
