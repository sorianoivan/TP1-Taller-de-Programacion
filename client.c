#include "client.h"
#include "common_message.h"

#define BUFF_SIZE 32
#define RESPONSE_LEN 3

#define ERROR -1
#define OK 0

#define STDIN "stdin"
#define READ "r"

#define CLIENT "client"

static void _client_initialize(client_t* client) {
    client->client_skt = 0;
}

static void _client_destroy(client_t* client){
    shutdown(client->client_skt, SHUT_RDWR);
    close(client->client_skt);
}

static int _send_message(const char* msg, int skt, int full_msg_len){
    if (try_send(msg, full_msg_len, skt) != 0){
        return ERROR;
    }
    return OK;
}

static int _store_line(char** line, FILE* file) {
    int bytes_read;
    char buff[BUFF_SIZE + 1];
    int bytes_written = 0;

    *line = malloc(BUFF_SIZE + 1);
    memset(*line, 0, BUFF_SIZE + 1);
    do {
        if (fgets(buff, BUFF_SIZE + 1, file) != NULL){
            bytes_read = (int)strlen(buff);
            *line = realloc(*line, strlen(*line) + bytes_read + 1);
            snprintf(*line + bytes_written, BUFF_SIZE + 1,"%s",buff);
            bytes_written += BUFF_SIZE;
        } else {
            return ERROR;
        }
    }while(bytes_read == BUFF_SIZE);
    return OK;
}

static int _read_line(FILE* file, client_t client, u_int32_t msg_id){
    int s = 0;
    char* line = NULL;
    char* msg = NULL;
    int full_msg_len = 0;

    if(_store_line(&line, file) == -1) return ERROR;

    msg = process_line(line, &full_msg_len, msg_id);
    s = _send_message(msg, client.client_skt, full_msg_len);

    free(msg);
    free(line);
    return s;
}

static int _recv_response(int skt, u_int32_t msg_id){
    char response[RESPONSE_LEN];
    if (try_recv(response,RESPONSE_LEN, skt) == -1){
        return ERROR;
    } else {
        printf("0x%08x: %s\n",msg_id, response);
    }
    return OK;
}

static int _process_file(FILE* file, client_t client) {
    u_int32_t msg_id = 1;
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

int client_start(const char* host, const char* port, const char* filename) {
    client_t client;
    FILE* file;
    int flag;

    _client_initialize(&client);

    if (set_up_connection(host, port, &client.client_skt, CLIENT) != 0) return ERROR;

    file = _open_input(filename);
    if (file == NULL){
        fprintf(stderr, "Error: %s\n", strerror(errno));
        return ERROR;
    }

    flag = _process_file(file, client);

    if (strcmp(filename, STDIN) != 0) fclose(file);
    _client_destroy(&client);
    return flag;
}
