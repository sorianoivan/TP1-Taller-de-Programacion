#define _POSIX_C_SOURCE 200112L

#include "common_message.h"

#define HEADER_INFO_SIZE 5
#define PARAMS_INFO_SIZE 4
#define FIRM_PARAMS_INFO_SIZE 2
#define FIRM_PARAMS_TYPE_SIZE 2

void separate_line(message_t* msg, int* padding, int* len, char* line){
    char* saveptr;

    msg->dest = strtok_r(line, " (", &saveptr);
    //printf("dest:%s\n", msg->dest);
    *len += (int)strlen(msg->dest);
    *padding += (int)(8 - (strlen(msg->dest) + 1) % 8) % 8;

    msg->path = strtok_r(NULL, " (", &saveptr);
    //printf("path:%s\n", msg->path);
    *len += (int)strlen(msg->path);
    *padding += (int)(8 - (strlen(msg->path) + 1) % 8) % 8;

    msg->interface = strtok_r(NULL, " (", &saveptr);
    //printf("interface:%s\n", msg->interface);
    *len += (int)strlen(msg->interface);
    *padding += (int)(8 - (strlen(msg->interface) + 1)% 8) % 8;

    msg->method = strtok_r(NULL, "(", &saveptr);
    //printf("method:%s\n", msg->method);
    *len += (int)strlen(msg->method);
    *padding += (int)(8 - (strlen(msg->method) + 1) % 8) % 8;

    msg->parameters = strtok_r(NULL, ")\n", &saveptr);
    //printf("parameters:%s\n", msg->parameters);
}

char* process_line(char* line, int* len, int id) {
    message_t msg;
    int padding = 0;
    int firma_len = 0;
    int curr_len = 0;
    int header_bytes_written = 0;
    int body_bytes_written = 0;
    char* header = NULL; // Meter esto en el struct asi libero la memoria aca
    char* body = NULL;
    int body_len = 0;
    int cant_param = 0;
    char* saveptr;

    separate_line(&msg, &padding, len, line);

    char* curr_param = NULL;

    if (msg.parameters != NULL){
        curr_param = strtok_r(msg.parameters, ", )", &saveptr);
    }
    while (curr_param != NULL && strcmp(curr_param, "") != 0){
        cant_param++;
        curr_len = (int)strlen(curr_param);
        body_len += (curr_len + 4 + 1);
        body = realloc(body, body_len + curr_len + 4 + 1); //param + largo + \0
        memcpy(body + body_bytes_written, &curr_len, 4);
        body_bytes_written += 4;
        memcpy(body + body_bytes_written, curr_param, curr_len + 1);
        body_bytes_written += curr_len + 1;
        curr_param = strtok_r(NULL, ",)", &saveptr);
    }

    if (cant_param != 0){
        firma_len = 5 + 2*cant_param;
    }

    int padding_firma = (8 - (firma_len) % 8) % 8; //padding de firma

    padding += padding_firma;

    *len += padding + 16 + 8*4 + 4 + firma_len;
    //16 bytes fijos + 8 fijos por 4 parametros + 4 * \0 + firma

    header = malloc(*len + 1);
    memset(header, 0, *len + 1);
    snprintf(header,HEADER_INFO_SIZE, "l%c%c%c",1,0,1);
    header_bytes_written += 4;

    memcpy(header + header_bytes_written, &body_len, 4);
    header_bytes_written += 4;
    memcpy(header + header_bytes_written, &id, 4);
    //meto ahora el id dos veces pero en realidad aca va body_len
    header_bytes_written += 4;

    int header_len = *len - 16 - padding_firma;
    memcpy(header + header_bytes_written, &header_len, 4);
    header_bytes_written += 4;

    snprintf(header + header_bytes_written, PARAMS_INFO_SIZE,
            "%c%c%s", 1, 1, "o");
    header_bytes_written += 4;
    curr_len = (int)strlen(msg.path);
    memcpy(header + header_bytes_written, &curr_len, 4);
    header_bytes_written += 4;
    strncpy(header + header_bytes_written, msg.path, curr_len + 1);
    header_bytes_written += (int)strlen(msg.path) + 1 +
            (int)(8 - (strlen(msg.path) + 1) % 8) % 8;

    snprintf(header + header_bytes_written, PARAMS_INFO_SIZE,
            "%c%c%s", 6, 1, "s");
    header_bytes_written += 4;
    curr_len = (int) strlen(msg.dest);
    memcpy(header + header_bytes_written, &curr_len, 4);
    header_bytes_written += 4;
    strncpy(header + header_bytes_written, msg.dest, curr_len + 1);
    header_bytes_written += (int)strlen(msg.dest) + 1 +
            (int)(8 - (strlen(msg.dest) + 1) % 8) % 8;

    snprintf(header + header_bytes_written, PARAMS_INFO_SIZE,
            "%c%c%s", 2, 1, "s");
    header_bytes_written += 4;
    curr_len = (int) strlen(msg.interface);
    memcpy(header + header_bytes_written, &curr_len, 4);
    header_bytes_written += 4;
    strncpy(header + header_bytes_written, msg.interface, curr_len + 1);
    header_bytes_written += (int)strlen(msg.interface) + 1 +
            (int)(8 - (strlen(msg.interface) + 1) % 8) % 8;

    snprintf(header + header_bytes_written, PARAMS_INFO_SIZE,
            "%c%c%s", 3, 1, "s");
    header_bytes_written += 4;
    curr_len = (int) strlen(msg.method);
    memcpy(header + header_bytes_written, &curr_len, 4);
    header_bytes_written += 4;
    strncpy(header + header_bytes_written, msg.method, curr_len + 1);
    header_bytes_written += (int)strlen(msg.method) + 1 +
            (int)(8 - (strlen(msg.method) + 1) % 8) % 8;

    if (cant_param != 0){
        snprintf(header + header_bytes_written, PARAMS_INFO_SIZE,
                "%c%c%s", 8, 1, "g");
        header_bytes_written += 4;
        snprintf(header + header_bytes_written, FIRM_PARAMS_INFO_SIZE,
                "%c", cant_param);
        header_bytes_written += 1;
        for (int i = 0; i < cant_param; ++i) {
            snprintf(header + header_bytes_written, FIRM_PARAMS_TYPE_SIZE,
                    "%s", "s");
            header_bytes_written += 2;
        }
        header_bytes_written += padding_firma;

        *len += body_len;
        header = realloc(header, *len);
        memcpy(header + header_bytes_written,body, body_len);

        free(body);
    }
    return header;
}


