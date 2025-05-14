#include<stdio.h>
#include<string.h>
#include<sys/types.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>
#include<signal.h>
#include "MCP.h"

void alarm_handler(int sig);

int find_next_index();

int finished_processes = 0;
int curr_index = 0;
int num_processes;
int* pids;

int main(int argc, char** argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: no input file\n");
        return 1;
    }

    num_processes = count_lines(argv[1]);
    struct command* file_array = read_file(argv[1],num_processes);
    pid_t pid; 
    pid_t parent = getpid();
    pids = malloc(sizeof(pid_t) * num_processes);

    for (int i = 0; i < num_processes; i++) {
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

    for(int i = 0; i < num_processes; i++)
    {
        kill(pids[i],SIGUSR1);
    }
    for(int i = 0; i < num_processes; i++)
    {
        kill(pids[i],SIGSTOP);
    }
    kill(pids[0],SIGUSR1);

    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGALRM);
    
    signal(SIGALRM, alarm_handler);
    alarm(1);
    int count = 0;
    while(count != 100)
    {
        int status;
        pid_t result = waitpid(pids[curr_index], &status, WNOHANG);
        if (WIFSTOPPED(status))
        {
            printf("Process %d is paused\n", pids[curr_index]);
        }
        else
        {
            printf("Process %d is finished\n",pids[curr_index]);
            finished_processes++;
        }

        sigprocmask(SIG_BLOCK, &set, NULL);
        curr_index = find_next_index();
        kill(pids[curr_index],SIGCONT);
        if(finished_processes == num_processes-1)
        {   
            waitpid(pids[curr_index], NULL, 0);
            break;
        }
        sigprocmask(SIG_UNBLOCK, &set, NULL);
        count++;    
    }
    printf("process %d finished all processes finished\n",pids[curr_index]);
    printf("finished_processes: %d\n",finished_processes);
    
    free(pids);
    free_command_array(file_array,num_processes);
    return 0;
}


void alarm_handler(int sig){
    kill(pids[curr_index],SIGSTOP);
    printf("pausing process %d\n",pids[curr_index]);
    alarm(1);
}
    

int find_next_index(){
    int found = -1;
    for(int i=curr_index+1;i<num_processes;i++)
    {
        if (pids[i] != -1)
        {
            found = i;
            break;
        }
    }
    if(found == -1)
    {   for(int i=0;i<curr_index;i++)
        {
            if (pids[i] != -1)
            {
                found = i;
                break;
            }
        }  
    }
    return found;  
}
