#include "server.h"
#include <arpa/inet.h>
#include <byteswap.h>


#define ERROR -1
#define OK 0
#define SERVER "server"

#define SOCKET_CLOSED 0

#define HEADER_INFO_SIZE 16

#define MAX_LISTENERS 1

static void _server_initialize(server_t* server){
    server->bind_skt = 0;
    server->peer_skt = 0;
}

static void _server_destroy(server_t* server){
    shutdown(server->bind_skt, SHUT_RDWR);
    shutdown(server->peer_skt, SHUT_RDWR);
    close(server->bind_skt);
    close(server->peer_skt);
}

static int _calculate_msg_len(char** msg, int* id, u_int32_t* body_len) {
    int bytes_read = 0;
    u_int32_t header_len = 0;
    int padding_header = 0;

    bytes_read += 4; //ignoro los primeros 4 bytes
    *body_len = *( (uint32_t *)(*msg + bytes_read));
    *body_len = bswap_32(ntohl(*body_len));
    bytes_read += 4;
    *id = *(*msg + bytes_read); //id
    bytes_read += 4;
    header_len = *((uint32_t *)(*msg + bytes_read) );//largo del body
    padding_header = (int)(8 - (header_len) % 8) % 8;

    return (int)(header_len + *body_len + padding_header);
}

static int _recv_info(char** msg, int skt, int* id, u_int32_t* body_len) {
    int recieved = 0;

    *msg = malloc(HEADER_INFO_SIZE);
    memset(*msg, 0, HEADER_INFO_SIZE);

    recieved = try_recv(*msg, HEADER_INFO_SIZE, skt);
    if (recieved == 0){
        free(*msg);
        return SOCKET_CLOSED;
    }
    if (recieved == -1){
        free(*msg);
        return ERROR;
    }

    return _calculate_msg_len(msg, id, body_len);
}

static int recv_message(char** msg, int skt, int* id, u_int32_t* body_len){
    int msg_len = _recv_info(msg,skt, id, body_len);
    if (msg_len <= 0) return msg_len;

    *msg = realloc(*msg, msg_len);
    memset(*msg, 0, msg_len);

    return  try_recv(*msg, msg_len, skt);
}

void _set_header_parameters(u_int32_t* bytes_read, char* msg, char** param){
    u_int32_t curr_param_len = 0;
    u_int32_t curr_padding = 0;

    *bytes_read += 4; //no me importan los primeros 4
    curr_param_len = *( (uint32_t *) (msg + *bytes_read));
    *bytes_read += 4;
    curr_padding = (8 - (curr_param_len + 1) % 8) % 8;
    *param = (msg + *bytes_read);
    *bytes_read += (int)(curr_param_len + curr_padding + 1);
}
static void _print_header(u_int32_t* bytes_read, message_t* msg_to_print, char* msg, int id){
    printf("* Id: 0x%08x\n", id);

    _set_header_parameters(bytes_read, msg, &msg_to_print->path);
    _set_header_parameters(bytes_read, msg, &msg_to_print->dest);
    _set_header_parameters(bytes_read, msg, &msg_to_print->interface);
    _set_header_parameters(bytes_read, msg, &msg_to_print->method);

    printf("* Destino: %s\n", msg_to_print->dest);
    printf("* Ruta: %s\n", msg_to_print->path);
    printf("* Interfaz: %s\n", msg_to_print->interface);
    printf("* Metodo: %s\n", msg_to_print->method);
}

static void _print_body(u_int32_t body_len, u_int32_t* bytes_read, char* msg){
    u_int32_t curr_padding = 0;
    u_int32_t curr_param_len = 0;

    if (body_len != 0){
        printf("* Parametros:\n");
        *bytes_read += 4;//ignoro los primeros 4 de la firma
        char cant_param = *(msg + *bytes_read);
        curr_padding = (8 - (5 + 2*cant_param) % 8) % 8;
        *bytes_read += (2*(u_int32_t)cant_param + curr_padding + 1);
        for (int i = 0; i < cant_param; ++i) {
            curr_param_len = *((uint32_t *) (msg + *bytes_read));
            *bytes_read += 4;
            printf("    * %s\n", (msg + *bytes_read));
            *bytes_read += curr_param_len + 1;
        }
    }
    printf("\n");
}

void show_message(char* msg, const int id, u_int32_t body_len){
    u_int32_t bytes_read = 0;
    message_t msg_to_print;

    _print_header(&bytes_read, &msg_to_print, msg, id);
    _print_body(body_len, &bytes_read, msg);
}

static int _server_listen(int bind_skt){
    int peer_skt;
    if (listen(bind_skt, MAX_LISTENERS) == -1) return ERROR;

    peer_skt = accept(bind_skt, NULL, NULL);
    if (peer_skt == -1){
        fprintf(stderr, "Error: %s\n", strerror(errno));
        return ERROR;
    }
    return peer_skt;
}

static int _send_response(int peer_skt){
    if (try_send("OK", 3, peer_skt) != 0){
        return ERROR;
    }
    return 0;
}

static int _process_msg(int peer_skt) {
    int recieved = 0;
    int id = 0;
    u_int32_t body_len = 0;
    char* msg = NULL;
    do {
        recieved = recv_message(&msg, peer_skt, &id, &body_len);
        if (recieved == -1) return ERROR;
        if (recieved != 0){
            show_message(msg, id, body_len);
            free(msg);
            if (_send_response(peer_skt) == -1) return ERROR;
        }
    }while(recieved != 0);
    return 0;
}

int server_start(const char* port){
    server_t server;
    int flag;

    _server_initialize(&server);

    if (set_up_connection(NULL,port, &server.bind_skt, SERVER) != 0) return ERROR;

    server.peer_skt = _server_listen(server.bind_skt);
    if (server.peer_skt == -1) return ERROR;

    flag = _process_msg(server.peer_skt);

    _server_destroy(&server);

    return flag;
}
