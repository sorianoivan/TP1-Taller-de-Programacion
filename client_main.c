#include "client.h"

#define ERROR -1
#define STDIN "stdin"

int main(int argc, const char *argv[]) {
    if (argc == 3){
        return client_start(argv[1], argv[2], STDIN);
    } else if (argc == 4) {
        return client_start(argv[1], argv[2], argv[3]);
    } else {
        fprintf(stderr, "Argumentos Invalidos\n");
        return ERROR;
    }
}


