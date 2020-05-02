#include "server.h"

#define ERROR -1

void server_initialize(server_t* server){
    server->bind_skt = 0;
    server->peer_skt = 0;
}

char* recv_message(char* msg, int* skt,
        int* recieved, int* id, u_int32_t * body_len){
    int bytes_read = 0;
    u_int32_t header_len = 0;

    msg = malloc(16); //capaz le tengo q sumar 1
    memset(msg, 0, 16);

    *recieved = try_recv(msg, 16, skt);        //lee la primera linea del header
    bytes_read += 4; //ignoro los primeros 4 bytes

    *body_len = *( (uint32_t *) (msg + bytes_read)); //largo del body
    bytes_read += 4;

    *id = *(msg + bytes_read); //id
    bytes_read += 4;

    header_len = *( (uint32_t *) (msg + bytes_read) );//largo del body

    int padding_firma = (int)(8 - (header_len) % 8) % 8;

    msg = realloc(msg,header_len + *body_len + padding_firma);
    memset(msg, 0, header_len + *body_len + padding_firma);

    *recieved = try_recv(msg, header_len + *body_len + padding_firma, skt);

    return msg;
}

void show_message(char* msg, const int id, u_int32_t body_len){
    u_int32_t curr_param_len = 0;
    u_int32_t curr_padding = 0;
    u_int32_t bytes_read = 0;

    message_t msg_to_print;

    printf("* Id: 0x%08X\n", id);

    bytes_read += 4; //no me importan los primeros 4
    curr_param_len = *( (uint32_t *) (msg + bytes_read));
    bytes_read += 4;
    curr_padding = (8 - (curr_param_len + 1) % 8) % 8;
    //printf("* Path: %s\n", (msg + bytes_read));
    msg_to_print.path = (msg + bytes_read);
    bytes_read += curr_param_len + curr_padding + 1;

    bytes_read += 4; //no me importan los primeros 4
    curr_param_len = *( (uint32_t *) (msg + bytes_read));
    bytes_read += 4;
    curr_padding = (8 - (curr_param_len + 1) % 8) % 8;
    //printf("* Destino: %s\n", (msg + bytes_read));
    msg_to_print.dest = (msg + bytes_read);
    bytes_read += curr_param_len + curr_padding + 1;

    bytes_read += 4; //no me importan los primeros 4
    curr_param_len = *( (uint32_t *) (msg + bytes_read));
    bytes_read += 4;
    curr_padding = (8 - (curr_param_len + 1) % 8) % 8;
    //printf("* Interfaz: %s\n", (msg + bytes_read));
    msg_to_print.interface = (msg + bytes_read);
    bytes_read += curr_param_len + curr_padding + 1;

    bytes_read += 4; //no me importan los primeros 4
    curr_param_len = *( (uint32_t *) (msg + bytes_read));
    bytes_read += 4;
    curr_padding = (8 - (curr_param_len + 1) % 8) % 8;
    //printf("* Método: %s\n", (msg + bytes_read));
    msg_to_print.method = (msg + bytes_read);
    bytes_read += curr_param_len + curr_padding + 1;

    printf("* Destino: %s\n", msg_to_print.dest);
    printf("* Path: %s\n", msg_to_print.path);
    printf("* Interfaz: %s\n", msg_to_print.interface);
    printf("* Método: %s\n", msg_to_print.method);

    //u_int32_t header_bytes_read = bytes_read;

    if (body_len != 0){
        printf("* Parametros:\n");
        bytes_read += 4;//ignoro los primeros 4 de la firma
        char cant_param = *(msg + bytes_read);
        curr_padding = (8 - (5 + 2*cant_param) % 8) % 8;
        bytes_read += 2*(u_int32_t)cant_param + curr_padding + 1;
        for (int i = 0; i < cant_param; ++i) {
            curr_param_len = *((uint32_t *) (msg + bytes_read));
            bytes_read += 4;
            printf("    * %s\n", (msg + bytes_read));
            bytes_read += curr_param_len + 1;
        }
    }
    printf("\n");
}

int server_start(const char* port){
    server_t server;
    server_initialize(&server);

    if (server_bind(port, &server.bind_skt) != 0){
        return ERROR;
    }
    listen(server.bind_skt, 5);
    server.peer_skt = accept(server.bind_skt, NULL, NULL);

    int recieved = 0;
    int id = 0;
    u_int32_t body_len = 0;
    char* msg = NULL;
    do {
        msg = recv_message(msg, &server.peer_skt, &recieved, &id, &body_len);
        if (recieved == -1){
            return -1;
        }
        if (recieved == 0){
            break;
        }
        show_message(msg, id, body_len);
        /*for (int i = 0; i < recieved; ++i) {
            printf("%c", *(msg + i));
        }*/

        if (msg != NULL){
          free(msg);
        }
        if (try_send("OK", 3, &server.peer_skt) != 0){
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
