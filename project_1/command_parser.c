#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "command.h"
#include "string_parser.h"
#include "command_parser.h"

int num_args(char* input) {
    int counter = 0;
    for (int i = 0; i < strlen(input); i++) {
        if (i == 0 && input[i] != ' ') {
            counter+=1;
        } else if (input[i] != ' ' && input[i - 1] == ' ') {
            counter+=1;
        }
    }
    return counter;
}


char** parse_command(char*input,int num_commands){
    char** parsed = (char**)malloc(num_commands * sizeof(char*));
    int i = 0;
    char* token = strtok(input, " ");

    while (token != NULL) {
        parsed[i] = (char*)malloc(strlen(token) + 1);
        strcpy(parsed[i], token);
        token = strtok(NULL, " ");
        i++;
    }
    
    return parsed;
}


void free_parsed(char**parsed,int num_args){
    for(int i=0;i<num_args;i++){
        free(parsed[i]);
    }
    free(parsed);
}