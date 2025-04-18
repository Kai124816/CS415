#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "string_parser.h"

int count_token(char* buf, const char* delim){
    if (buf == NULL){
        return 0;
    }

    char* token; char* ptr;

    int count = 0;

    char* copy_buf = (char*)malloc(sizeof(char) * strlen(buf) + 1);
    strcpy(copy_buf, buf);

    copy_buf[strlen(buf)] = '\0';

    token = strtok_r(copy_buf, delim, &ptr);
    while(token != NULL){
        count++;
        token = strtok_r(NULL, delim, &ptr);
    }

    return count;
}


command_line str_filler(char* buf, const char* delim){
    command_line cmd_line;
    char* token;
    char* ptr;

    cmd_line.num_token = count_token(buf, delim);
    cmd_line.command_list = (char**)malloc(cmd_line.num_token * sizeof(char*));

    char* copy_buf = (char*)malloc(strlen(buf) + 1);
    strcpy(copy_buf, buf);

    int i = 0;
    token = strtok_r(copy_buf, delim, &ptr);
    while(token != NULL){
        cmd_line.command_list[i] = (char*)malloc(strlen(token) + 1);
        strcpy(cmd_line.command_list[i], token);
        token = strtok_r(NULL, delim, &ptr);
        i++;
    }

    free(copy_buf);
    return cmd_line;
}


void free_command_line(command_line* command){
    for(int i=0; i<command->num_token; i++){
        free(command->command_list[i]);
    }
    free(command->command_list);
}