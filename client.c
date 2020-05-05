#include "client.h"

#define ERROR -1
#define OK 0

#define BUFF_SIZE 33
#define RESPONSE_LEN 3

#define STDIN "stdin"
#define READ "r"

#define CLIENT "client"

void client_initialize(client_t* self) {
    self->client_skt = 0;
}

static int _send_message(const char* msg, const int skt,
        const int full_msg_len){
    if (try_send(msg, full_msg_len, skt) != 0){
        return ERROR;
    }
    return OK;
}

static int _store_line(char** line, FILE* file) {
    int bytes_read, bytes_written = 0;
    char buff[BUFF_SIZE];

    *line = malloc(33*sizeof(char));
    memset(*line, 0, 33*sizeof(char));
    do {
        if (fgets(buff, BUFF_SIZE, file) != NULL){
            bytes_read = (int)strlen(buff);
            *line = realloc(*line, strlen(*line) + bytes_read + 1);
            snprintf(*line + bytes_written, BUFF_SIZE,"%s",buff);
            bytes_written += BUFF_SIZE - 1;
        } else {
            return ERROR;
        }
    }while(bytes_read == BUFF_SIZE - 1);
    return OK;
}

static int _read_line(FILE* file, client_t client, const uint32_t msg_id){
    int flag = 0,  full_msg_len = 0;
    char* line = NULL;
    char* msg = NULL;

    if (_store_line(&line, file) == -1){
        free(line);
        return ERROR;
    }

    msg = process_line(line, &full_msg_len, msg_id);
    flag = _send_message(msg, client.client_skt, full_msg_len);

    free(msg);
    free(line);
    return flag;
}

static int _recv_response(const int skt, const uint32_t msg_id){
    char response[RESPONSE_LEN];

    if (try_recv(response,RESPONSE_LEN, skt) == -1){
        return ERROR;
    } else {
        print_response(msg_id, response);
    }
    return OK;
}

static int _process_file(FILE* file, client_t client) {
    uint32_t msg_id = 1;

    while (!feof(file)){
        if (_read_line(file, client, msg_id) != 0) return ERROR;

        if (_recv_response(client.client_skt, msg_id) != 0) return ERROR;

        msg_id++;
    }
    return OK;
}

static FILE* _open_input(const char* filename){
    if (!strcmp(filename, STDIN)){
        return stdin;
    } else {
        return fopen(filename, READ);
    }
}

int client_start(client_t * self, const char* host, const char* port,
        const char* filename) {
    FILE* file;
    int flag;

    if (set_up_connection(host, port, &self->client_skt, CLIENT) != 0)
        return ERROR;

    file = _open_input(filename);
    if (file == NULL){
        fprintf(stderr, "Error: %s\n", strerror(errno));
        return ERROR;
    }

    flag = _process_file(file, *self);

    if (strcmp(filename, STDIN) != 0) fclose(file);
    return flag;
}

void client_destroy(client_t* self){
    shutdown(self->client_skt, SHUT_RDWR);
    close(self->client_skt);
}
