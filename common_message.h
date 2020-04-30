#ifndef TP1_COMMON_MESSAGE_H
#define TP1_COMMON_MESSAGE_H

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

typedef struct {
    char* dest;
    char* path;
    char* interface;
    char* method;
    char* parameters;
    char* firm;
} message_t;

char* process_line(char* line, int* len, int id);

#endif //TP1_COMMON_MESSAGE_H
