#include<stdio.h>
#include<string.h>
#include<sys/types.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>
#include "MCP.h"


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
    return 0;
}

