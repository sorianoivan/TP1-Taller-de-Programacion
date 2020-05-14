#define _POSIX_C_SOURCE 200112L

#include "common_socket.h"

#define CLIENT "client"
#define SERVER "server"

#define ERROR -1
#define OK 0

#define SOCKET_CLOSED 0

static int _set_addrinfo(struct addrinfo** results, const char* host,
        const char* port, const char* mode) {
    struct addrinfo hints;
    int flag = 0;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    if (!strcmp(mode,CLIENT)){
        hints.ai_flags = 0;
    } else {
        hints.ai_flags = AI_PASSIVE;
    }
    flag = getaddrinfo(host, port, &hints, results);
    if (flag != 0){
        fprintf(stderr, "Error: %s\n", gai_strerror(flag));
        return ERROR;
    }
    return flag;
}

static int _try_connect(struct addrinfo* results, int* skt) {
    struct addrinfo* current_result;
    bool connected = false;

    for (current_result = results; current_result != NULL
        && connected == false; current_result = current_result->ai_next) {
        *skt = socket(current_result->ai_family,
                current_result->ai_socktype, current_result->ai_protocol);
        if (*skt == -1){
            fprintf(stderr,"Error: %s\n", strerror(errno));
            return ERROR;
        }
        if (connect(*skt, current_result->ai_addr,
                current_result->ai_addrlen) == -1) {
            fprintf(stderr,"Error: %s\n", strerror(errno));
            close(*skt);
            return ERROR;
        } else {
            connected = true;
        }
    }
    freeaddrinfo(results);
    return OK;
}

/* Esta funcion se pasa del limite de 20 lineas ya que hace llamadas a
 * funciones que reciben parametros largos, lo que causa que dichas
 * llamadas ocupen mas de 80 caracteres por lo cual debo repartirlas
 * en mas lineas */
static int _try_bind(struct addrinfo* results, int* skt) {
    struct addrinfo* current_result;
    bool connected = false;

    for (current_result = results; current_result != NULL
        && connected == false; current_result = current_result->ai_next) {
        *skt = socket(current_result->ai_family,
                      current_result->ai_socktype,
                      current_result->ai_protocol);
        if (*skt == -1) {
            fprintf(stderr, "Error: %s\n", strerror(errno));
            return ERROR;
        }
        int val = 1;
        if (setsockopt(*skt, SOL_SOCKET, SO_REUSEADDR,
                &val, sizeof(val)) == -1) return ERROR;
        if (bind(*skt, current_result->ai_addr,
                 current_result->ai_addrlen) == -1) {
            fprintf(stderr, "Error: %s\n", strerror(errno));
            close(*skt);
            return ERROR;
        } else {
            connected = true;
        }
    }
    freeaddrinfo(results);
    return OK;
}

void socket_initialize(int* skt){
    *skt = 0;
}

int socket_set_up_connection(const char* host, const char* port, int* skt,
                             const char* mode){
    struct addrinfo *results;
    memset(&results, 0, sizeof(struct addrinfo*));

    int flag = _set_addrinfo(&results, host, port, mode);
    if (flag != 0){
        freeaddrinfo(results);
        return ERROR;
    }

    if (!strcmp(mode,SERVER)) {
        return _try_bind(results, skt);
    } else {
        return _try_connect(results, skt);
    }
}

int socket_send(const void* msg, int msg_len, const int skt){
    int bytes_sent = 0;
    int sent = 0;

    while (bytes_sent < msg_len) {
        sent = send(skt, msg, msg_len - bytes_sent, MSG_NOSIGNAL);
        if (sent == -1){
            fprintf(stderr,"Error: %s\n", strerror(errno));
            return ERROR;
        }
        if (sent == 0) return 0;
        bytes_sent += sent;
    }
    return OK;
}

int socket_receive(void* buff, uint32_t buff_len, const int skt){
    int received = 0;
    int bytes_received = 0;

    while (bytes_received < buff_len) {
        received = recv(skt, buff, buff_len - bytes_received, 0);
        if (received == -1){
            fprintf(stderr,"Error: %s\n", strerror(errno));
            return ERROR;
        }
        if (received == 0) return 0;

        bytes_received += received;
    }
    return received;
}

void socket_destroy(int* skt){
    shutdown(*skt, SHUT_RDWR);
    close(*skt);
}

