#include "socket.h"


int client_connect(const char* host, const char* port, int* skt){

    int flag = 0;
    bool connected = false;

    struct addrinfo hints;
    struct addrinfo* results;
    struct addrinfo* current_result;


    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;       /* IPv4 (or AF_INET6 for IPv6)     */
    hints.ai_socktype = SOCK_STREAM; /* TCP  (or SOCK_DGRAM for UDP)    */
    hints.ai_flags = 0;              /* None (or AI_PASSIVE for server) */

    flag = getaddrinfo(host, port, &hints, &results);


    if (flag != 0) {
        printf("Error in getaddrinfo: %s\n", gai_strerror(flag));
        return 1;
    }

    for (current_result = results; current_result != NULL && connected == false; current_result = current_result->ai_next) {
        *skt = socket(current_result->ai_family, current_result->ai_socktype, current_result->ai_protocol);
        if (*skt == -1) {
            printf("Error: %s\n", strerror(errno));
        }
        else
        {
            flag = connect(*skt, current_result->ai_addr, current_result->ai_addrlen);
            if (flag == -1) {
                printf("Error: %s\n", strerror(errno));
                close(*skt);
            }
            connected = (flag != -1); // nos conectamos?
        }
    }
    freeaddrinfo(results);

    return 0;
}


int server_bind(const char* port, int* skt){
    int flag;
    bool bound = false;

    struct addrinfo hints;
    struct addrinfo* results;
    struct addrinfo* current_result;


    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;    /* Allow IPv4 or IPv6 */
    hints.ai_socktype = SOCK_STREAM; /* Datagram socket */
    hints.ai_flags = AI_PASSIVE;    /* For wildcard IP address */

    flag = getaddrinfo(NULL, port, &hints, &results);
    if (flag != 0) {
        //printf("Error in getaddrinfo: %s\n", gai_strerror(flag));
        printf("getaddrinfo");
        return 1;
    }

    int val = 1;

    for (current_result = results; current_result != NULL && bound == false; current_result = current_result->ai_next) {
        *skt = socket(current_result->ai_family, current_result->ai_socktype, current_result->ai_protocol);
        if (*skt == -1){
            //printf("Error: %s\n", strerror(errno));
            printf("socket");
        }
        setsockopt(*skt, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));

        if (bind(*skt, current_result->ai_addr, current_result->ai_addrlen) != 0){
            //printf("Error: %s\n", strerror(errno));
            printf("bind");
            close(*skt);
        } else {
            bound = true;
        }
    }

    freeaddrinfo(results);

    return 0;

}

int send_message(const char* msg, int* skt){

    int sent;
    int len = (int)strlen(msg);
    int bytesSentTotal = 0;
    //bool error = false;

    printf("length:%d\n", len);

    while (bytesSentTotal < 4) {
        sent = send(*skt, &len, 4 - bytesSentTotal, MSG_NOSIGNAL);
        if (sent == -1){
            printf("error send length");
            return -1;
        }
        bytesSentTotal += sent;
    }

    bytesSentTotal = 0;

    while (bytesSentTotal < len) {
        sent = send(*skt, msg, len - bytesSentTotal, MSG_NOSIGNAL);
        if (sent == -1) return -1;;
        bytesSentTotal += sent;
    }

    return 0;
}

int recv_message(char* buf, int* skt){

    int s = 0;
    int bytesRecieved = 0;
    int len = 0;
    while (bytesRecieved < 4 && s != -1) {
        s = recv(*skt, &len, 4 - bytesRecieved, 0);
        if (s!=-1) bytesRecieved += s;
    }
    //int len = atoi(length);
    //printf("%d\n", len);

    bytesRecieved = 0;
    char* buff = malloc(len + 1);
    memset(buff, 0, len + 1);
    while (bytesRecieved < len && s != -1) {
        s = recv(*skt, buff, len - bytesRecieved, 0);
        //printf("%d\n", s );
        if (s!=-1) bytesRecieved += s;
    }
    printf("%s\n", buff);

    free(buff);

    return 0;

}
