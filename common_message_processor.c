#include "common_message_processor.h"
#include <arpa/inet.h>
#include <byteswap.h>

#define HEADER_FULL_INFO_SIZE 16
#define HEADER_FIXED_INFO_SIZE 5
#define HEADER_FIXED_PARAMS 4
#define PARAMS_TYPE_INFO_SIZE 4
#define FULL_PARAM_INFO_SIZE 8
#define FIRMA_PARAMS_INFO_SIZE 2
#define FIRMA_PARAMS_TYPE_SIZE 2
#define INFO_FIRMA 5

#define ALINEACION 8

static void _set_header_param(char** param, char** pos_actual,
        const char* delim, int* len, int* padding){
    char* aux;
    aux = strstr(*pos_actual, delim);
    *aux = '\0';
    *param = *pos_actual;
    *pos_actual = (aux + 1);
    *len += (int)strlen(*param);
    *padding += (int)(ALINEACION - (strlen(*param) + 1)
            % ALINEACION) % ALINEACION;
}

static void _set_body_parameters(char** parameters, char* pos_actual,
        const char* delim){
    char* aux;
    aux = strstr(pos_actual, delim);
    *aux = '\0';
    *parameters = pos_actual;
}

/* Asigna a cada variable la parte del mensaje que le corresponde*/
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
    if (aux != NULL){
        *aux = '\0';
        *params = *pos_actual;
        curr_param = *pos_actual;
        *pos_actual = (aux + 1);
        return curr_param;
    }
    *params = *pos_actual;
    *pos_actual = NULL;
    return *params;
}

/* La funcion supera las 20 lineas ya que tiene varios comentarios explicando
 * la funcionalidad que agregan lineas. Ademas, si bien la logica que sigue
 * no es simple, no veo razon para modularizarla mas todavia */
static void _build_body(char** body, char* parameters, int* body_len,
        int* padding, int* cant_param, int* firma_len, int* padding_firma) {
    char* curr_param = NULL;
    char* pos_actual = parameters;
    int curr_len = 0, body_bytes_written = 0;

    curr_param = _get_param(&parameters, &pos_actual, ",");//Leo un parametro
    while (curr_param != NULL && strcmp(curr_param, "") != 0){
        *cant_param += 1;
        curr_len = (int)strlen(curr_param);
        curr_len = mp_host_to_little(curr_len);
        *body_len += (curr_len + 4 + 1);//parametro + 4 bytes que representan
                                        //el largo del parametro + \0
        *body = realloc(*body, *body_len);
        //Escribo los 4 bytes que represental el largo del parametro
        memcpy(*body + body_bytes_written, &curr_len,sizeof(int) );
        body_bytes_written += 4;//Me muevo a donde debo escribir el parametro
        //Escribo el parametro
        memcpy(*body + body_bytes_written, curr_param, curr_len + 1);
        body_bytes_written += curr_len + 1;//Me muevo al final del parametro
        if (pos_actual != NULL)
            //Vuelvo a leer un parametro si es que todavia quedan
            curr_param = _get_param(&parameters, &pos_actual, ",");
        else
            curr_param = NULL;
    }
    if (*cant_param != 0) *firma_len = INFO_FIRMA + 2 * (*cant_param);
    *padding_firma = (ALINEACION - (*firma_len) % ALINEACION) % ALINEACION;
    *padding += *padding_firma;
}

/* La funcion supera las 20 lineas ya que tiene varios comentarios explicando
 * la funcionalidad que agregan lineas. */
static void _add_header_info(char** header, int len, int body_len, int id,
        int* header_bytes_written, int padding_firma){
    int header_len = 0;

    *header = malloc((len + 1) * sizeof(char));
    memset(*header, 0, (len + 1) * sizeof(char));
    //Escribo info del header
    snprintf(*header, HEADER_FIXED_INFO_SIZE, "l%c%c%c",1,0,1);
    *header_bytes_written += 4;
    //Escribo el largo del body
    body_len = mp_host_to_little(body_len);
    memcpy(*header + *header_bytes_written, &body_len, 4);
    *header_bytes_written += 4;
    //Escribo el id
    id = mp_host_to_little(id);
    memcpy(*header + *header_bytes_written, &id, 4);
    *header_bytes_written += 4;

    header_len = len - HEADER_FULL_INFO_SIZE - padding_firma;
                                            //Para el largo del header no
                                            // cuentan los primeros 16 bytes
                                            // ni el padding de la firma
    header_len = mp_host_to_little(header_len);
    //Escribo el largo del header
    memcpy(*header + *header_bytes_written, &header_len, 4);
    *header_bytes_written += 4;
}

static void _add_header_param(char** header,
        int* header_bytes_written, char* param,
        char param_type, char* data_type){
    int curr_len = 0;

    snprintf(*header + *header_bytes_written, PARAMS_TYPE_INFO_SIZE,
             "%c%c%s", param_type, 1, data_type);//Escribo info del parametro
    *header_bytes_written += 4;
    curr_len = (int)strlen(param);
    curr_len = mp_host_to_little(curr_len);
    //Escribo el largo del parametro actual
    memcpy(*header + *header_bytes_written, &curr_len, 4);
    *header_bytes_written += 4;
    //Escribo el parametro en el header
    strncpy(*header + *header_bytes_written, param, curr_len + 1);
    *header_bytes_written += (int)strlen(param) + 1 +
            (int)(ALINEACION - (strlen(param) + 1) % ALINEACION) % ALINEACION;
}

static char* _build_header(int id, int* len, int body_len,
                           int* header_bytes_written, int padding_firma,
                           message_processor_t msg){
    char* header;

    _add_header_info(&header, *len, body_len, id,
            header_bytes_written, padding_firma);
    _add_header_param(&header, header_bytes_written, msg.path, 1, "o");
    _add_header_param(&header, header_bytes_written, msg.dest, 6, "s");
    _add_header_param(&header, header_bytes_written, msg.interface, 2, "s");
    _add_header_param(&header, header_bytes_written, msg.method, 3, "s");

    return header;
}

static void _add_body(int cant_param, char** header, char* body,
        int* header_bytes_written, int padding_firma, int body_len, int* len){
    snprintf(*header + *header_bytes_written, PARAMS_TYPE_INFO_SIZE,
             "%c%c%s", 8, 1, "g");//Escribo la informacion de la firma
    *header_bytes_written += 4;
    snprintf(*header + *header_bytes_written, FIRMA_PARAMS_INFO_SIZE,
             "%c", cant_param);//Escribo la cantidad de parametros de la firma
    *header_bytes_written += 1;
    for (int i = 0; i < cant_param; ++i) {
        snprintf(*header + *header_bytes_written, FIRMA_PARAMS_TYPE_SIZE,
                 "%s", "s");//Escribo "s" por cada parametro de la firma
        *header_bytes_written += 2;
    }
    *header_bytes_written += padding_firma;
    *len += body_len;
    *header = realloc(*header, *len);
    //Escribo el body despues del header
    memcpy(*header + *header_bytes_written, body, body_len);
    free(body);
}

int mp_host_to_little(int n) {
    return bswap_32(htonl(n));
}

int mp_little_to_host(int n) {
    return ntohl(bswap_32(n));
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

    *len += padding + HEADER_FULL_INFO_SIZE +
            FULL_PARAM_INFO_SIZE * HEADER_FIXED_PARAMS + firma_len + 4;
    //El 4 sumado al final se debe a los \0 de cada parametro

    header = _build_header(id, len, body_len, &header_bytes_written,
            padding_firma, msg);
    if (cant_param != 0) _add_body(cant_param, &header, body,
            &header_bytes_written, padding_firma, body_len, len);

    return header;
}


