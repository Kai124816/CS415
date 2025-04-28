#include<stdio.h>
#include<string.h>
#include <sys/types.h>
#include<stdlib.h>
#include<unistd.h>
#include <sys/wait.h>

struct command {
    char** arg_array;
    int arg_count;
};

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


struct command create_command(char*input){
    int num_args = count_args(input);
    int i = 0;
    char** parsed = (char**)malloc((num_args+1) * sizeof(char*));
    char* token = strtok(input, " ");
    struct command c1;

    while (token != NULL) {
        parsed[i] = (char*)malloc(strlen(token) + 1);
        strcpy(parsed[i], token);
        token = strtok(NULL, " ");
        i++;
    }
    
    parsed[num_args] = NULL;
    c1.arg_array = parsed;
    c1.arg_count = num_args;
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
}


int main(int argc, char** argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: no input file\n");
        return 1;
    }

    int n = count_lines(argv[1]);
    struct command* file_array = read_file(argv[1],n);
    pid_t pid; 

    int* pids = malloc(sizeof(pid_t) * n);

    for (int i = 0; i < n; i++) {
        pid = fork();
        if (pid == 0) {
            // child process
            printf("I am the child process. My PID: %d\n", getpid());
            execvp(file_array[i].arg_array[0], file_array[i].arg_array);

            // if execvp fails
            perror("execvp failed");
            exit(EXIT_FAILURE);
        }
        else if (pid > 0) {
            // parent process
            printf("I am the parent process. The child had PID: %d\n", pid);
            pids[i] = pid;
        }
        else {
            perror("fork fail");
        }
    }

    for (int i = 0; i < n; i++) {
        // wait for children by pids
        waitpid(pids[i], NULL, 0);
    }

    free(pids);
    free_command_array(file_array,n);
    free(file_array);
    return 0;
}
