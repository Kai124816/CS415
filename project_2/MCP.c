#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include"MCP.h"

int count_args(char*input)
{
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


struct command create_command(char *input) {
    struct command c1;
    int i = 0;

    char *copy = strdup(input); 
    char *token = strtok(copy, " \t\n");

    int capacity = 8;
    char **parsed = malloc(capacity * sizeof(char *));

    while (token != NULL) {
        if (i >= capacity) {
            capacity *= 2;
            parsed = realloc(parsed, capacity * sizeof(char *));
        }

        parsed[i] = strdup(token);  // mallocs and copies token
        i++;
        token = strtok(NULL, " \t\n");
    }

    parsed[i] = NULL;
    c1.arg_array = parsed;
    c1.arg_count = i;

    free(copy);
    return c1;
}



void free_command(struct command c1){
    for(int i=0; i<c1.arg_count; i++){
        free(c1.arg_array[i]);
    }
    free(c1.arg_array);
}


int count_lines(char* filename){
    FILE *file = fopen(filename, "r");  
    if (file == NULL) {
        perror("Error opening file");
        return 1;
    }

    char line[1024];
    int counter = 0;

    while (fgets(line, sizeof(line), file)) {
        counter++;
    }

    fclose(file); 
    return counter;
}


struct command* read_file(char* filename,int num_lines) {
    FILE *file = fopen(filename, "r");  
    if (file == NULL) {
        perror("Error opening file");
        return 1;
    }

    char line[1024];
    struct command* file_array = (struct command*)malloc(num_lines * sizeof(struct command));
    int counter = 0;

    while (fgets(line, sizeof(line), file)) {
        file_array[counter] = create_command(line);
        counter++;
    }

    fclose(file);  
    return file_array;
}


void free_command_array(struct command* command_array, int line_count){
    for(int i=0; i<line_count; i++){
        free_command(command_array[i]);
    }
    free(command_array);
}
