#ifndef TP1_COMMON_MESSAGE_PROCESSOR_H
#define TP1_COMMON_MESSAGE_PROCESSOR_H

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

typedef struct {
    char* dest;
    char* path;
    char* interface;
    char* method;
    char* parameters;
} message_processor_t;

/* Recibe una linea del archivo y arma el mensaje siguiendo el protocolo DBus
 * Devuelve un puntero al inicio del mensaje armado */
char* mp_process_line(char* line, int* len, int id);

/* Recibe un numero y lo pasa a little endian */
int mp_host_to_little(int n);

/* recibe un numero en little endian y lo pasa al endianness del host */
int mp_little_to_host(int n);

#endif //TP1_COMMON_MESSAGE_PROCESSOR_H
