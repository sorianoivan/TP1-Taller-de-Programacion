#include "client.h"

#define ERROR -1

void client_initialize(client_t* client) {
    client->client_skt = 0;
}

int client_start(const char* host, const char* port, const char* filename) {

    client_t client;
    client_initialize(&client);

    if(client_connect(host, port, &client.client_skt) != 0) {     //ver si puedo sacar el != 0
        printf("Error conectando cliente");
        return ERROR;
    }

    if(send_message(filename, &client.client_skt) != 0){
        printf("error send");
        return -1;
    }

    close(client.client_skt);

    return 0;

    /*char* mensaje = "hola";
    int* length;
    int len = strlen(argv[2]);
    length = &len;
    int bytesSent = 0;

    printf("len:%d\n", len);
    printf("length:%d\n", *length);

    while (bytesSent < 4) {
        s = send(skt, length, 4 - bytesSent, MSG_NOSIGNAL);
        if (s == -1) break;
        bytesSent += s;
    }

    bytesSent = 0;

    while (bytesSent < *length) {
        s = send(skt, argv[2], *length - bytesSent, MSG_NOSIGNAL);
        if (s == -1) break;
        bytesSent += s;
    }

    close(skt);

    return 0;*/

}
