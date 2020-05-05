#include "server.h"

#define ERROR -1

int main(int argc, char const *argv[]) {
    server_t server;
    int flag = 0;

    server_initialize(&server);

    if (argc != 2){
        fprintf(stderr, "Argumentos Invalidos\n");
        return ERROR;
    }

    flag = server_start(&server, argv[1]);

    server_destroy(&server);

    return flag;
}
