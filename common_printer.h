#ifndef _COMMON_PRINTER_H
#define _COMMON_PRINTER_H

#include <stdio.h>
#include <sys/types.h>

#include "common_message.h"

/* Imprime por pantalla Id, Destino, Ruta, Interfaz y Metodo */
void print_header(u_int32_t* bytes_read, message_t* msg_to_print,
        char* msg, int id);

/* Imprime por pantalla los parametros del mensaje */
void print_body(u_int32_t body_len, u_int32_t* bytes_read, char* msg);

/* Imprime por pantalla la respuesta recibida */
void print_response(u_int32_t id, char* response);

#endif //_COMMON_PRINTER_H
