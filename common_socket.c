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

int set_up_connection(const char* host, const char* port, int* skt,
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

int try_send(const void* msg, int msg_len, const int skt){
    int bytesSentTotal = 0;
    int sent;

    while (bytesSentTotal < msg_len) {
        sent = send(skt, msg, msg_len - bytesSentTotal, MSG_NOSIGNAL);
        if (sent == -1){
            fprintf(stderr,"Error: %s\n", strerror(errno));
            return ERROR;
        }
        bytesSentTotal += sent;
    }
    return OK;
}

int try_recv(void* buff, u_int32_t buff_len, const int skt){
    int recieved = 0;
    int bytesRecieved = 0;

    while (bytesRecieved < buff_len) {
        recieved = recv(skt, buff, buff_len - bytesRecieved, 0);
        if (recieved == -1){
            fprintf(stderr,"Error: %s\n", strerror(errno));
            return ERROR;
        }
        if (recieved == 0){
            return 0;
        }
        bytesRecieved += recieved;
    }
    return recieved;
}

