#include "common_socket.h"

int _set_addrinfo(struct addrinfo** results, const char* host, const char* port, const char* mode) {
    struct addrinfo hints;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;       /* IPv4 (or AF_INET6 for IPv6)     */
    hints.ai_socktype = SOCK_STREAM; /* TCP  (or SOCK_DGRAM for UDP)    */

    if(!strcmp(mode,"client")){
        hints.ai_flags = 0;     /* None (or AI_PASSIVE for server) */
    } else {
        hints.ai_flags = AI_PASSIVE;
    }

    return getaddrinfo(host, port, &hints, results);
}

int _try_connect(struct addrinfo* results, int* skt, const char* mode){
    struct addrinfo* current_result;
    bool connected = false;

    for (current_result = results; current_result != NULL && connected == false; current_result = current_result->ai_next) {
        *skt = socket(current_result->ai_family, current_result->ai_socktype, current_result->ai_protocol);
        if (*skt == -1){
            printf("Error: %s\n", strerror(errno));
            return -1;
        }
        if(!strcmp(mode,"server")){
            int val = 1;
            setsockopt(*skt, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));

            if (bind(*skt, current_result->ai_addr, current_result->ai_addrlen) != 0){
                printf("Error: %s\n", strerror(errno));
                close(*skt);
                return -1;
            } else {
                connected = true;                                                                           //CAMBIAR ESTO POR PUNTERO A FUNCION
            }                                                                                               // CONNECT O BIND

        } else {
            if (connect(*skt, current_result->ai_addr, current_result->ai_addrlen) == -1) {
                printf("Error: %s\n", strerror(errno));
                close(*skt);
                return -1;
            } else {
                connected = true;
            }
        }
    }

    freeaddrinfo(results);
    return 0;
} //ARREGLAR ESTO

int client_connect(const char* host, const char* port, int* skt){
    struct addrinfo *results;
    memset(&results, 0, sizeof(struct addrinfo*));

    if (_set_addrinfo(&results, host, port, "client") != 0) {
        printf("Error in getaddrinfo");
        return 1;
    }
    return _try_connect(results, skt, "client");
}

int server_bind(const char* port, int* skt){
    struct addrinfo *results;
    memset(&results, 0, sizeof(struct addrinfo*));

    if (_set_addrinfo(&results, NULL, port, "server") != 0) {
        printf("Error in getaddrinfo");
        return 1;
    }
    return _try_connect(results, skt, "server");
}

int try_send(const void* msg, int msg_len, const int* skt){
    int sent = 0;
    int bytesSentTotal = 0;

    while (bytesSentTotal < msg_len) {
        sent = send(*skt, msg, msg_len - bytesSentTotal, MSG_NOSIGNAL);
        if (sent == -1){
            printf("error send\n");
            return -1;
        }
        bytesSentTotal += sent;
    }
    return 0;
}

int try_recv(void* buff, int buff_len, const int* skt){
    int recieved = 0;
    int bytesRecieved = 0;

    while (bytesRecieved < buff_len) {
        recieved = recv(*skt, buff, buff_len - bytesRecieved, 0);
        if (recieved == -1){
            printf("error recv\n");
            return -1;
        }
        if(recieved == 0){
            return 0;
        }
        bytesRecieved += recieved;
    }
    return recieved;
}

