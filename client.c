#include "client.h"
#include "common_message.h"

#define ERROR -1

void client_initialize(client_t* client) {
    client->client_skt = 0;
}

int send_message(const char* msg, int* skt, int full_msg_len){
    //int len = (int)strlen(msg);
    if(try_send(&full_msg_len, sizeof(int), skt) != 0){
        printf("error send");
        return -1;
    }

    if(try_send(msg, full_msg_len, skt) != 0){
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

    FILE* file = fopen(filename, "r");
    char* line;
    char buff[33];
    int read = 0;
    int s = 0;
    int id = 1;

    line = malloc(33);
    memset(line, 0, 33);

    char* msg = NULL;
    int full_msg_len = 0;

    while (!feof(file)){
        do {
            if(fgets(buff, 33, file) == NULL){
                break;
            }
            read = (int)strlen(buff);
            if(strlen(line) != 0){
                line = realloc(line, strlen(line) + read + 1);
            }
            strcat(line, buff);
        }while(read == 32);

        if(strcmp(line, "") != 0){
            msg = process_line(line, &full_msg_len, id);
            id++;
            s = send_message(msg, &client.client_skt, full_msg_len); //Ver lo de cuando se cierra el server
            memset(line, 0, strlen(line));

            free(msg);
            full_msg_len = 0;
        }
   }
    free(line);

    shutdown(client.client_skt, SHUT_RDWR);
    close(client.client_skt);

    return s;
}
