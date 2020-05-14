#include "common_message_processor.h"
#include <arpa/inet.h>
#include <byteswap.h>

#define HEADER_FIXED_INFO_SIZE 5
#define PARAMS_INFO_SIZE 4
#define FIRM_PARAMS_INFO_SIZE 2
#define FIRM_PARAMS_TYPE_SIZE 2

static void _set_header_param(char** param, char** pos_actual, const char* delim,
        int* len, int* padding){
    char* aux;
    aux = strstr(*pos_actual, delim);
    *aux = '\0';
    *param = *pos_actual;
    *pos_actual = (aux + 1);
    *len += (int)strlen(*param);
    *padding += (int)(8 - (strlen(*param) + 1) % 8) % 8;
}

static void _set_body_parameters(char** parameters, char* pos_actual,
        const char* delim){
    char* aux;
    aux = strstr(pos_actual, delim);
    *aux = '\0';
    *parameters = pos_actual;
}

static void _split_line(message_processor_t* msg,
        int* padding, int* len, char* line){
    char* pos_actual;
    pos_actual = line;

    _set_header_param(&msg->dest, &pos_actual, " ", len, padding);
    _set_header_param(&msg->path, &pos_actual, " ", len, padding);
    _set_header_param(&msg->interface, &pos_actual, " ", len, padding);
    _set_header_param(&msg->method, &pos_actual, "(", len, padding);

    _set_body_parameters(&msg->parameters, pos_actual, ")");
}

static char* _get_param(char** params, char** pos_actual, const char* delim){
    char* aux;
    char* curr_param;
    aux = strstr(*pos_actual, delim);
    if(aux != NULL){
        *aux = '\0';
        *params = *pos_actual;
        curr_param = *pos_actual;
        *pos_actual = (aux + 1);
        return curr_param;
    }
    *pos_actual = NULL;
    return *params;
}

static void _build_body(char** body, char* parameters, int* body_len,
        int* padding, int* cant_param, int* firma_len, int* padding_firma) {
    char* curr_param = NULL;
    char* pos_actual = parameters;
    int curr_len = 0, body_bytes_written = 0;

    curr_param = _get_param(&parameters, &pos_actual, ",");
    while (curr_param != NULL && strcmp(curr_param, "") != 0){
        *cant_param += 1;
        curr_len = (int)strlen(curr_param);
        curr_len = bswap_32(htonl(curr_len));
        *body_len += (curr_len + 4 + 1);
        *body = realloc(*body, *body_len + curr_len + 4 + 1);
        memcpy(*body + body_bytes_written, &curr_len, 4);
        body_bytes_written += 4;
        memcpy(*body + body_bytes_written, curr_param, curr_len + 1);
        body_bytes_written += curr_len + 1;
        if (pos_actual != NULL)
            curr_param = _get_param(&parameters, &pos_actual, ",");
        else curr_param = NULL;
    }
    if (*cant_param != 0) *firma_len = 5 + 2* (*cant_param);
    *padding_firma = (8 - (*firma_len) % 8) % 8;
    *padding += *padding_firma;
}

static void _add_header_info(char** header, int len, int body_len, int id,
        int* header_bytes_written, int padding_firma){
    int header_len = 0;

    *header = malloc(len + 1);
    memset(*header, 0, len + 1);

    snprintf(*header,HEADER_FIXED_INFO_SIZE, "l%c%c%c",1,0,1);
    *header_bytes_written += 4;

    body_len = bswap_32(htonl(body_len));
    memcpy(*header + *header_bytes_written, &body_len, 4);
    *header_bytes_written += 4;

    id = bswap_32(htonl(id));
    memcpy(*header + *header_bytes_written, &id, 4);
    *header_bytes_written += 4;

    header_len = len - 16 - padding_firma;
    header_len = bswap_32(htonl(header_len));
    memcpy(*header + *header_bytes_written, &header_len, 4);
    *header_bytes_written += 4;
}

static void _add_header_param(char** header,
        int* header_bytes_written, char* param){
    int curr_len = 0;

    snprintf(*header + *header_bytes_written, PARAMS_INFO_SIZE,
             "%c%c%s", 1, 1, "o");
    *header_bytes_written += 4;
    curr_len = (int)strlen(param);
    curr_len = bswap_32(htonl(curr_len));
    memcpy(*header + *header_bytes_written, &curr_len, 4);
    *header_bytes_written += 4;
    strncpy(*header + *header_bytes_written, param, curr_len + 1);
    *header_bytes_written += (int)strlen(param) + 1 +
                             (int)(8 - (strlen(param) + 1) % 8) % 8;
}

static char* _build_header(int id, int* len, int body_len,
                           int* header_bytes_written, int padding_firma,
                           message_processor_t msg){
    char* header;

    _add_header_info(&header, *len, body_len, id,
            header_bytes_written, padding_firma);
    _add_header_param(&header, header_bytes_written, msg.path);
    _add_header_param(&header, header_bytes_written, msg.dest);
    _add_header_param(&header, header_bytes_written, msg.interface);
    _add_header_param(&header, header_bytes_written, msg.method);

    return header;
}

static void _add_body(int cant_param, char** header, char* body,
        int* header_bytes_written, int padding_firma, int body_len, int* len){
    snprintf(*header + *header_bytes_written, PARAMS_INFO_SIZE,
             "%c%c%s", 8, 1, "g");
    *header_bytes_written += 4;
    snprintf(*header + *header_bytes_written, FIRM_PARAMS_INFO_SIZE,
             "%c", cant_param);
    *header_bytes_written += 1;
    for (int i = 0; i < cant_param; ++i) {
        snprintf(*header + *header_bytes_written, FIRM_PARAMS_TYPE_SIZE,
                 "%s", "s");
        *header_bytes_written += 2;
    }
    *header_bytes_written += padding_firma;
    *len += body_len;
    *header = realloc(*header, *len);
    memcpy(*header + *header_bytes_written, body, body_len);

    free(body);
}

char* mp_process_line(char* line, int* len, int id) {
    message_processor_t msg;
    int padding = 0, padding_firma = 0, cant_param = 0;
    int firma_len = 0, body_len = 0, header_bytes_written = 0;
    char *header = NULL;
    char *body = NULL;

    _split_line(&msg, &padding, len, line);
    if (msg.parameters != NULL)
    _build_body(&body, msg.parameters, &body_len, &padding,
                &cant_param, &firma_len, &padding_firma);

    *len += padding + 16 + 8*4 + 4 + firma_len;

    header = _build_header(id, len, body_len, &header_bytes_written,
            padding_firma, msg);
    if (cant_param != 0) _add_body(cant_param, &header, body,
            &header_bytes_written, padding_firma, body_len, len);

    return header;
}


