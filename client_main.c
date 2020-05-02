#include "client.h"


int main(int argc, const char *argv[]) {
    if (argc == 3){
        return client_start(argv[1], argv[2], "stdin");
    } else if (argc == 4) {
        return client_start(argv[1], argv[2], argv[3]);
    } else {
        return -1;
    }
}


