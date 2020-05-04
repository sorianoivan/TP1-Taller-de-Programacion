#include "server.h"

#define ERROR -1

int main(int argc, char const *argv[]) {
    server_t server;
    server_initialize(&server);

    if (argc != 2){
        fprintf(stderr, "Argumentos Invalidos\n");
        return ERROR;
    }

    server_destroy(&server);
    return server_start(&server, argv[1]);
}
