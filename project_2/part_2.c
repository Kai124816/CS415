#include<stdio.h>
#include<string.h>
#include<sys/types.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>
#include<signal.h>
#include "MCP.h"


int main(int argc, char** argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: no input file\n");
        return 1;
    }

    int n = count_lines(argv[1]);
    struct command* file_array = read_file(argv[1],n);
    pid_t pid; 
    pid_t parent = getpid();
    int* pids = malloc(sizeof(pid_t) * n);

    for (int i = 0; i < n; i++) {
        pid = fork();
        
        if(pid == 0){
            //Fork and wait for signal to execute program
            sigset_t set;
            sigemptyset(&set);
            sigaddset(&set, SIGUSR1);

            sigprocmask(SIG_BLOCK, &set, NULL);
            int sig;
            printf("process with pid %d wating to launch\n",getpid());
            sigwait(&set, &sig);
            execvp(file_array[i].arg_array[0], file_array[i].arg_array);
            exit(-1);
        }
        else if(pid > 0){
            pids[i] = pid;
        }
        else{
            perror("fork fail");
        }
    }
    sleep(2);
    for(int i=0; i<n; i++){
        printf("launching process %d\n",pids[i]);
        kill(pids[i],SIGUSR1);
    }
    sleep(2);

    for(int i=0; i<n; i++){
        printf("pausing process %d\n",pids[i]);
        kill(pids[i],SIGSTOP);
    }
    sleep(2);

    for(int i=0; i<n; i++){
        printf("continuing process %d\n",pids[i]);
        kill(pids[i],SIGCONT);
    }

    for (int i = 0; i < n; i++) {
        printf("waiting for process %d to finish\n",pids[i]);
        waitpid(pids[i], NULL, 0);
        printf("process %d finished\n",pids[i]);
    }

    free(pids);
    free_command_array(file_array,n);
    return 0;
}
