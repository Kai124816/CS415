#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> 
#include <fcntl.h>
#include <sys/stat.h>
#include "command.h"
#include "string_parser.h"
#include "command_parser.h"

int main(int argc, char *argv[]) {
    char* delim = ";";
    FILE *input = stdin;  
    int output = STDOUT_FILENO;
    char *filename = NULL;
    int file_mode = 0;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-f") == 0) {
            if (i + 1 < argc) {  
                filename = argv[i + 1];  
                file_mode = 1;
                i++;  
            } else {
                exit(EXIT_FAILURE);
            }
        } else {
            exit(EXIT_FAILURE);
        }
    }

    if(file_mode)
    {
        input = fopen(filename, "r");
        output = open("output.txt", O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
        dup2(output,STDOUT_FILENO);
    }

    char* ls = "ls";
    char* pwd = "pwd";
    char* mkdir = "mkdir";
    char* cd = "cd";
    char* cp = "cp";
    char* mv = "mv";
    char* rm = "rm";
    char* cat = "cat";
    char* ex = "exit";

    while(1) {
        char *line = NULL;
        size_t len = 0;
        ssize_t read;
        int exit = 0;

        if(!file_mode){
            write(output,">>> ",strlen(">>> "));
        }
        read = getline(&line, &len, input);
        if (read == -1) {
            if(line != NULL){free(line);}
            break;
        }
        trim_trailing_whitespace(line);
        command_line cmd = str_filler(line,delim);

        for(int i=0; i<cmd.num_token; i++){
            int empty = 0;
            int arguments = num_args(cmd.command_list[i]);
            if(arguments == 0){empty = 1;}
            char** parsed_command = parse_command(cmd.command_list[i],arguments);

            if (!empty && strcmp(ls,parsed_command[0]) == 0){
                if(arguments != 1){
                    char* errormessage = "Error! Unsupported parameters for command: ls\n";
                    write(1,errormessage,strlen(errormessage));
                }
                else{
                    listDir();
                }
            }
            else if (!empty && strcmp(pwd,parsed_command[0]) == 0){
                if(arguments != 1){
                    char* errormessage = "Error! Unsupported parameters for command: pwd\n";
                    write(1,errormessage,strlen(errormessage));
                }
                else{
                    showCurrentDir();
                }
            }
            else if (!empty && strcmp(mkdir,parsed_command[0]) == 0){
                if(arguments != 2){
                    char* errormessage = "Error! Unsupported parameters for command: mkdir\n";
                    write(1,errormessage,strlen(errormessage));
                }
                else{
                    makeDir(parsed_command[1]);
                }
            }
            else if (!empty && strcmp(cd,parsed_command[0]) == 0){
                if(arguments != 2){
                    char* errormessage = "Error! Unsupported parameters for command: cd\n";
                    write(1,errormessage,strlen(errormessage));
                }
                else{
                    changeDir(parsed_command[1]);
                }
            }
            else if (!empty && strcmp(cp,parsed_command[0]) == 0){
                if(arguments != 3){
                    char* errormessage = "Error! Unsupported parameters for command: cp\n";
                    write(1,errormessage,strlen(errormessage));
                }
                else{
                    copyFile(parsed_command[1],parsed_command[2]);
                }
            }
            else if (!empty && strcmp(mv,parsed_command[0]) == 0){
                if(arguments != 3){
                    char* errormessage = "Error! Unsupported parameters for command: mv\n";
                    write(1,errormessage,strlen(errormessage));
                }
                else{
                    moveFile(parsed_command[1],parsed_command[2]);
                }
            }
            else if (!empty && strcmp(rm,parsed_command[0]) == 0){
                if(arguments != 2){
                    char* errormessage = "Error! Unsupported parameters for command: rm\n";
                    write(1,errormessage,strlen(errormessage));
                }
                else{
                    deleteFile(parsed_command[1]);
                }
            }
            else if (!empty && strcmp(cat,parsed_command[0]) == 0){
                if(arguments != 2){
                    char* errormessage = "Error! Unsupported parameters for command: cat\n";
                    write(1,errormessage,strlen(errormessage));
                }
                else{
                    displayFile(parsed_command[1]);
                }
            }
            else if (!empty && strcmp(ex,parsed_command[0]) == 0){
                exit = 1;
                free_parsed(parsed_command,arguments);
                break;
            }
            else{
                if(!empty)
                {
                    char error[256];
                    snprintf(error, sizeof(error), "Error! Unrecognized command: %s\n", parsed_command[0]);
                    write(1,error,strlen(error));
                }
            }

            if(parsed_command != NULL){free_parsed(parsed_command,arguments);}
        }

        free_command_line(&cmd);
        free(line);
        if (exit == 1){break;}
    }

    if (file_mode) {
        close(output);
        fclose(input);
    }
}
