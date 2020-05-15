#include "common_printer.h"

#define ALINEACION 8

#define INFO_FIRMA 5

static void _set_header_parameters(u_int32_t* bytes_read,
        char* msg, char** param){
    u_int32_t param_len = 0;
    u_int32_t padding = 0;

    *bytes_read += 4;//Me muevo 4 bytes al largo del parametro
    param_len = *((u_int32_t*)(msg + *bytes_read));//Leo el largo del parametro
    *bytes_read += 4;//Me muevo 4 bytes al parametro
    padding = (ALINEACION - (param_len + 1) % ALINEACION) % ALINEACION;
    *param = (msg + *bytes_read);//Leo el parametro
    *bytes_read += (int)(param_len + padding + 1);
}

void printer_show_header(u_int32_t* bytes_read, char* msg, int id){
    message_processor_t msg_to_print;

    printf("* Id: 0x%08x\n", id);

    _set_header_parameters(bytes_read, msg, &msg_to_print.path);
    _set_header_parameters(bytes_read, msg, &msg_to_print.dest);
    _set_header_parameters(bytes_read, msg, &msg_to_print.interface);
    _set_header_parameters(bytes_read, msg, &msg_to_print.method);

    printf("* Destino: %s\n", msg_to_print.dest);
    printf("* Ruta: %s\n", msg_to_print.path);
    printf("* Interfaz: %s\n", msg_to_print.interface);
    printf("* Metodo: %s\n", msg_to_print.method);
}

void printer_show_body(u_int32_t body_len, u_int32_t* bytes_read, char* msg){
    u_int32_t padding = 0;
    u_int32_t param_len = 0;

    if (body_len != 0){
        printf("* Parametros:\n");
        *bytes_read += 4;//Me muevo 4 bytes a la cantidad de parametros
        char cant_param = *(msg + *bytes_read);//Leo la cantidad de parametros
        int firma_len = INFO_FIRMA + 2 * cant_param;
        padding = (ALINEACION - firma_len % ALINEACION) % ALINEACION;
        *bytes_read += (2 * cant_param + padding + 1);//Ignoro info de firma
        for (int i = 0; i < cant_param; ++i) {
            param_len = *((u_int32_t *)(msg + *bytes_read));
            *bytes_read += 4;//Ya lei el largo del parametro actual
            printf("    * %s\n", (msg + *bytes_read));
            *bytes_read += param_len + 1;//Lei el parametro con su \0
        }
    }
    printf("\n");
}

void printer_show_response(u_int32_t id, char* response){
    printf("0x%08x: %s\n",id, response);
}
