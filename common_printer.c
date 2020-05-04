#include "common_printer.h"

static void _set_header_parameters(u_int32_t* bytes_read,
        char* msg, char** param){
    u_int32_t curr_param_len = 0;
    u_int32_t curr_padding = 0;

    *bytes_read += 4;
    curr_param_len = *( (u_int32_t *) (msg + *bytes_read));
    *bytes_read += 4;
    curr_padding = (8 - (curr_param_len + 1) % 8) % 8;
    *param = (msg + *bytes_read);
    *bytes_read += (int)(curr_param_len + curr_padding + 1);
}

void print_header(u_int32_t* bytes_read, message_t* msg_to_print,
        char* msg, int id){
    printf("* Id: 0x%08x\n", id);

    _set_header_parameters(bytes_read, msg, &msg_to_print->path);
    _set_header_parameters(bytes_read, msg, &msg_to_print->dest);
    _set_header_parameters(bytes_read, msg, &msg_to_print->interface);
    _set_header_parameters(bytes_read, msg, &msg_to_print->method);

    printf("* Destino: %s\n", msg_to_print->dest);
    printf("* Ruta: %s\n", msg_to_print->path);
    printf("* Interfaz: %s\n", msg_to_print->interface);
    printf("* Metodo: %s\n", msg_to_print->method);
}

void print_body(u_int32_t body_len, u_int32_t* bytes_read, char* msg){
    u_int32_t curr_padding = 0;
    u_int32_t curr_param_len = 0;

    if (body_len != 0){
        printf("* Parametros:\n");
        *bytes_read += 4;
        char cant_param = *(msg + *bytes_read);
        curr_padding = (8 - (5 + 2*cant_param) % 8) % 8;
        *bytes_read += (2*(u_int32_t)cant_param + curr_padding + 1);
        for (int i = 0; i < cant_param; ++i) {
            curr_param_len = *((u_int32_t *) (msg + *bytes_read));
            *bytes_read += 4;
            printf("    * %s\n", (msg + *bytes_read));
            *bytes_read += curr_param_len + 1;
        }
    }
    printf("\n");
}

void print_response(u_int32_t id, char* response){
    printf("0x%08x: %s\n",id, response);
}
