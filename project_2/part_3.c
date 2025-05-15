#include<stdio.h>
#include<string.h>
#include<sys/types.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>
#include<signal.h>
#include "MCP.h"

void alarm_handler();

int find_next_index();

void print_array(int arr[], int size);

struct mcpstate{
    int finished_processes;
    int curr_index;
    int num_processes;
    int* pids;
    int* started;
    int* finished;
};

struct mcpstate c1;


int main(int argc, char** argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: no input file\n");
        return 1;
    }

    c1.finished_processes = 0;
    c1.curr_index = 0;
    c1.num_processes = count_lines(argv[1]);
    c1.pids = malloc(sizeof(pid_t) * c1.num_processes);
    c1.started = (int*)calloc(c1.num_processes, sizeof(int));
    c1.finished = (int*)calloc(c1.num_processes, sizeof(int));
    struct command* file_array = read_file(argv[1],c1.num_processes);
    pid_t pid; 
    pid_t parent = getpid();

    for (int i = 0; i < c1.num_processes; i++) {
        pid = fork();
        printf("pid:%d\n",pid);
        if(pid == 0){
            //Fork and wait for signal to execute program
            sigset_t set;
            sigemptyset(&set);
            sigaddset(&set, SIGUSR1);
            sigprocmask(SIG_BLOCK, &set, NULL);
            int sig;
            printf("process with pid %d wating to launch\n",getpid());
            sigwait(&set, &sig);
            printf("test\n");
            if (execvp(file_array[i].arg_array[0], file_array[i].arg_array) == -1) {
                exit(-1);    
            }
        }
        else if(pid > 0){
            c1.pids[i] = pid;
        }
        else{
            perror("fork fail");
        }
    }

    kill(c1.pids[0],SIGUSR1);
    c1.started[0] = 1;
    printf("launching process: %d\n",c1.pids[0]);

    signal(SIGALRM, alarm_handler);	
    alarm(1);
    int count = 0;
    while(count != 100 || c1.finished_processes < c1.num_processes)
    {
        int status;
        pid_t result = waitpid(c1.pids[c1.curr_index], &status, WUNTRACED);
        if(result == 0)
        {
            continue;
        }
        else if (WIFSTOPPED(status))
        {
            printf("Process %d is paused\n", c1.pids[c1.curr_index]);
        }
        else
        {
            printf("Process %d is finished\n",c1.pids[c1.curr_index]);
            c1.finished_processes++;
            c1.finished[c1.curr_index] = 1;
            alarm(0);
            alarm(1);
        }

        c1.curr_index = find_next_index();
        if(c1.started[c1.curr_index] == 0)
        {
            kill(c1.pids[c1.curr_index],SIGUSR1);
            c1.started[c1.curr_index] = 1;
            printf("launching process %d\n",c1.pids[c1.curr_index]);
        }
        else
        {
            kill(c1.pids[c1.curr_index],SIGCONT);
            printf("continuing process %d\n",c1.pids[c1.curr_index]);
        }

        if(c1.finished_processes == c1.num_processes-1)
        {   
            waitpid(c1.pids[c1.curr_index], NULL, 0);
            c1.finished_processes++;
            break;
        }

        count++;
        if(c1.finished_processes < c1.num_processes)
        {
            alarm(1); 
        }
    }
    printf("process %d finished - all processes finished\n",c1.pids[c1.curr_index]);
    
    free(c1.pids);
    free(c1.finished);
    free(c1.started);
    free_command_array(file_array,c1.num_processes);
    return 0;
}


void alarm_handler(){
    printf("current process: %d\n",c1.pids[c1.curr_index]);
    if(c1.finished[c1.curr_index] == 0)
    {
        kill(c1.pids[c1.curr_index],SIGSTOP);
    }
}
    

int find_next_index(){
    int found = -1;
    for(int i=c1.curr_index+1;i<c1.num_processes;i++)
    {
        if (c1.finished[i] != 0)
        {
            found = i;
            break;
        }
    }
    if(found == -1)
    {   for(int i=0;i<c1.curr_index;i++)
        {
            if (c1.finished[i] != 0)
            {
                found = i;
                break;
            }
        }  
    }
    return found;  
}


void print_array(int arr[], int size) {
    printf("[");
    for(int i = 0; i < size; i++) {
        printf("%d", arr[i]);
        if(i < size - 1) {
            printf(", ");
        }
    }
    printf("]\n");
}