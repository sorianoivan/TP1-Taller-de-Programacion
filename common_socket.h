#ifndef _SOCKET_H
#define _SOCKET_H

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

/* Trata de establecer conexion con un socket. Si es exitoso devuelve 0,
 * en caso contrario devuelve -1*/
int set_up_connection(const char* host, const char* port,
        int* skt, const char* mode);

/* Itera sobre la funcion recv() para asegurar que se reciben todos los bytes.
 * Devuelve -1 si hay algun error, 0 si se cerro el socket que envia y,
 * en caso contrario, la cantidad de bytes recibidos */
int try_recv(void* buff, u_int32_t buff_len, const int skt);

/* Itera sobre la funcion send() para asegurar que se envian todos los bytes.
 * Devuelve -1 si hay algun error y, en caso contrario,
 * la cantidad de bytes enviados*/
int try_send(const void* msg, int msg_len, const int skt);

#endif //_SOCKET_H
