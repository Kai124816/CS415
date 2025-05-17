#include<stdio.h>
#include<string.h>
#include<sys/types.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>
#include<signal.h>
#include <time.h>
#include "MCP.h"

void alarm_handler();

int find_next_index();

void print_array(int arr[], int size);

void print_proc_info(pid_t pid);

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
        if(pid == 0){
            //Fork and wait for signal to execute program
            sigset_t set;
            sigemptyset(&set);
            sigaddset(&set, SIGUSR1);
            sigprocmask(SIG_BLOCK, &set, NULL);
            int sig;
            printf("process with pid %d wating to launch\n",getpid());
            sigwait(&set, &sig);
            if (execvp(file_array[i].arg_array[0], file_array[i].arg_array) == -1) {
                exit(EXIT_FAILURE);    
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
    clock_t start, end;
    double cpu_time_used;
    printf("launching process: %d\n",c1.pids[0]);

    int cycle = 1;
    signal(SIGALRM, alarm_handler);	
    alarm(1);
    while (c1.finished_processes < c1.num_processes) {
        int status;
        pid_t result = waitpid(c1.pids[c1.curr_index], &status, WUNTRACED);
    
        if (result == 0) {
            continue;
        } else if (result == -1) {
            perror("waitpid failed");
            break;
        }
    
        if (WIFSTOPPED(status)) {
            printf("Process %d is paused\n", c1.pids[c1.curr_index]);
        } else {
            printf("Process %d is finished\n", c1.pids[c1.curr_index]);
            c1.finished_processes++;
            c1.finished[c1.curr_index] = 1;
            alarm(0);
            alarm(1);  // Reschedule alarm to trigger soon after process finishes
        }
    
        c1.curr_index = find_next_index();
    
        if (c1.started[c1.curr_index] == 0) {
            printf("Launching new process %d\n", c1.pids[c1.curr_index]);
            kill(c1.pids[c1.curr_index], SIGUSR1);
            c1.started[c1.curr_index] = 1;
        } else {
            printf("Resuming process %d\n", c1.pids[c1.curr_index]);
            kill(c1.pids[c1.curr_index], SIGCONT);
        }
    
        if (c1.finished_processes == c1.num_processes - 1) {
            printf("Only one process remaining. Waiting for pid %d to finish...\n", c1.pids[c1.curr_index]);
            waitpid(c1.pids[c1.curr_index], NULL, 0);
            c1.finished_processes++;
            break;
        }
        
        printf("Cycle: %d ----------------------\n",cycle);
        for(int i=0; i < c1.num_processes; i++)
        {   
            if(c1.finished[i] != 1)
            {
                print_proc_info(c1.pids[i]);
            }
        }
        printf("--------------------------------\n");
        cycle++;

        if (c1.finished_processes < c1.num_processes) {
            alarm(1);
        }
    }
    
    printf("process %d finished - all processes finished\n",c1.pids[c1.curr_index]);
    printf("process %d finished - all processes finished\n",c1.pids[c1.curr_index]);
    end = clock();
    cpu_time_used = ((double) (end - start)/1000);
    printf("Time taken: %f seconds\n", cpu_time_used);
    
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
        if (c1.finished[i] == 0)
        {
            found = i;
            break;
        }
    }
    if(found == -1)
    {   for(int i=0;i<c1.curr_index;i++)
        {
            if (c1.finished[i] == 0)
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


void print_proc_info(pid_t pid) {
    char path_stat[256], path_statm[256];
    FILE *fstat = NULL, *fstatm = NULL;

    snprintf(path_stat, sizeof(path_stat), "/proc/%d/stat", pid);
    snprintf(path_statm, sizeof(path_statm), "/proc/%d/statm", pid);

    fstat = fopen(path_stat, "r");
    if (!fstat) {
        perror("fopen stat");
        return;
    }

    int unused_pid, ppid;
    char comm[256], state;
    unsigned long utime, stime;

    fscanf(fstat, "%d (%[^)]) %c", &unused_pid, comm, &state);
    for (int i = 0; i < 10; ++i) fscanf(fstat, "%*s"); 
    fscanf(fstat, "%lu %lu", &utime, &stime);
    for (int i = 0; i < 2; ++i) fscanf(fstat, "%*s"); 
    fscanf(fstat, "%d", &ppid); 

    fclose(fstat);

    fstatm = fopen(path_statm, "r");
    if (!fstatm) {
        perror("fopen statm");
        return;
    }

    long total_pages, resident_pages;
    fscanf(fstatm, "%ld %ld", &total_pages, &resident_pages);
    fclose(fstatm);

    // Convert values
    long page_size_kb = sysconf(_SC_PAGESIZE) / 1024;  // in KB
    long clock_ticks = sysconf(_SC_CLK_TCK);
    double cpu_time = (utime + stime) / (double)clock_ticks;
    long memory_kb = resident_pages * page_size_kb;

    // Print info
    printf("PID: %d, Parent PID: %d, CMD: %s, State: %c, CPU Time: %lu (utime) + %lu (stime), Memory(kb): %ld\n",
           pid, ppid, comm, state, utime, stime, memory_kb);
}
