#ifndef _COMMON_PRINTER_H
#define _COMMON_PRINTER_H

#include <stdio.h>
#include <sys/types.h>

#include "common_message_processor.h"

/* Imprime por pantalla Id, Destino, Ruta, Interfaz y Metodo */
void printer_show_header(u_int32_t* bytes_read, char* msg, int id);

/* Imprime por pantalla los parametros del mensaje */
void printer_show_body(u_int32_t body_len, u_int32_t* bytes_read, char* msg);

/* Imprime por pantalla la respuesta recibida */
void printer_show_response(u_int32_t id, char* response);

#endif //_COMMON_PRINTER_H
