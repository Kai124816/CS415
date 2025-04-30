#include<stdio.h>
#include<string.h>
#include<sys/types.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>
#include<signal.h>

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
    free(command_array);
}


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
        printf("waiting for process %d\n to finish",pids[i]);
        waitpid(pids[i], NULL, 0);
    }

    free(pids);
    free_command_array(file_array,n);
    return 0;
}
