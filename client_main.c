#include "client.h"

#define ERROR -1
#define STDIN "stdin"

int main(int argc, const char *argv[]) {
    int flag = 0;
    client_t client;
    client_initialize(&client);

    if (argc == 3){
        flag = client_start(&client, argv[1], argv[2], STDIN);
    } else if (argc == 4) {
        flag = client_start(&client, argv[1], argv[2], argv[3]);
    } else {
        fprintf(stderr, "Argumentos Invalidos\n");
        return ERROR;
    }

    client_destroy(&client);
    return flag;
}


