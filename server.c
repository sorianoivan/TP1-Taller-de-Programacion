#include "server.h"

#define SERVER "server"

#define ERROR -1
#define OK 0

#define SOCKET_CLOSED 0
#define MAX_LISTENERS 1

#define RESPONSE_LEN 3

#define HEADER_INFO_SIZE 16

#define ALINEACION 8

void server_initialize(server_t* server){
    socket_initialize(&server->bind_skt);
    socket_initialize(&server->peer_skt);
}

static int _calculate_msg_len(const char* info, int* id, uint32_t* body_len) {
    int bytes_read = 0;
    int padding_header = 0;
    uint32_t header_len = 0;

    bytes_read += 4;//Ignoro los primeros 4 bytes ya que no son necesarios
    *body_len = *((uint32_t*)(info + bytes_read));
    *body_len = mp_little_to_host(*body_len);
    bytes_read += 4;//Ya lei el largo del body asi que me muevo 4 bytes
    *id = *(info + bytes_read);
    *id = mp_little_to_host(*id);
    bytes_read += 4;//Ya lei el ID asi que me muevo 4 bytes
    header_len = *((uint32_t*)(info + bytes_read));
    header_len = mp_little_to_host(header_len);
    padding_header = (int)(ALINEACION - header_len % ALINEACION) % ALINEACION;

    return (int)(header_len + *body_len + padding_header);
}

static int _recv_info(int skt, int* id, uint32_t* body_len) {
    int received = 0;
    char info[HEADER_INFO_SIZE];

    received = socket_receive(info, HEADER_INFO_SIZE, skt);
    if (received == 0) return SOCKET_CLOSED;
    if (received == -1) return ERROR;

    return _calculate_msg_len(info, id, body_len);
}

static int _recv_message(char** msg, int skt, int* id, uint32_t* body_len){
    int msg_len = _recv_info(skt, id, body_len);
    if (msg_len <= 0) return msg_len;

    *msg = realloc(*msg, (msg_len) * sizeof(char));
    memset(*msg, 0, (msg_len) * sizeof(char));

    return socket_receive(*msg, msg_len, skt);
}

static void _show_message(char* msg, const int id, uint32_t body_len){
    uint32_t bytes_read = 0;

    printer_show_header(&bytes_read, msg, id);
    printer_show_body(body_len, &bytes_read, msg);
}

static int _server_listen(int bind_skt){
    int peer_skt;
    if (listen(bind_skt, MAX_LISTENERS) == -1) return ERROR;

    peer_skt = accept(bind_skt, NULL, NULL);
    if (peer_skt == -1){
        fprintf(stderr, "Error: %s\n", strerror(errno));
        return ERROR;
    }
    return peer_skt;
}

static int _send_response(int peer_skt){
    if (socket_send("OK", RESPONSE_LEN, peer_skt) != 0){
        return ERROR;
    }
    return 0;
}

static int _process_msg(int peer_skt) {
    int received = 0;
    int id = 0;
    uint32_t body_len = 0;
    char* msg = NULL;
    do {
        received = _recv_message(&msg, peer_skt, &id, &body_len);
        if (received == -1) return ERROR;
        if (received != 0){
            _show_message(msg, id, body_len);
            if (_send_response(peer_skt) == -1) return ERROR;
        }
    }while(received != 0);
    free(msg);
    return 0;
}

int server_start(server_t* self, const char* port){
    int flag;

    if (socket_set_up_connection(NULL, port, &self->bind_skt, SERVER) != 0)
        return ERROR;

    self->peer_skt = _server_listen(self->bind_skt);
    if (self->peer_skt == -1) return ERROR;

    flag = _process_msg(self->peer_skt);

    return flag;
}

void server_destroy(server_t* server){
    socket_destroy(&server->bind_skt);
    socket_destroy(&server->peer_skt);
}
