#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "command.h"
#include "string_parser.h"
#include "command_parser.h"

int main() {
    char* delim = ";";

    while(1) {
        char *line = NULL;
        size_t len = 0;
        ssize_t read;
        int exit = 0;

        write(1,">>>",strlen(">>>"));
        read = getline(&line, &len, stdin);
        command_line cmd = str_filler(read,delim);

        for(int i=0; i<cmd.num_token; i++){
            char** parsed_command = parse_command(cmd.command_list[i]);
            int arguments = num_args(cmd.command_list[i]);
            if (parsed_command[0] == "ls"){
                if(arguments != 1){
                    char* errormessage = "Error! Unsupported parameters for command: ls";
                    write(1,errormessage,strlen(errormessage));
                }
                else{
                    listDir();
                }
            }
            else if (parsed_command[0] == "cwd"){
                if(arguments != 1){
                    char* errormessage = "Error! Unsupported parameters for command: cwd";
                    write(1,errormessage,strlen(errormessage));
                }
                else{
                    showCurrentDir();
                }
            }
            else if (parsed_command[0] == "mkdir"){
                if(arguments != 2){
                    char* errormessage = "Error! Unsupported parameters for command: mkdir";
                    write(1,errormessage,strlen(errormessage));
                }
                else{
                    makeDir(parsed_command[1]);
                }
            }
            else if (parsed_command[0] == "cd"){
                if(arguments != 2){
                    char* errormessage = "Error! Unsupported parameters for command: cd";
                    write(1,errormessage,strlen(errormessage));
                }
                else{
                    changeDir(parsed_command[1]);
                }
            }
            else if (parsed_command[0] == "cp"){
                if(arguments != 3){
                    char* errormessage = "Error! Unsupported parameters for command: cp";
                    write(1,errormessage,strlen(errormessage));
                }
                else{
                    copyFile(parsed_command[1],parsed_command[2]);
                }
            }
            else if (parsed_command[0] == "mv"){
                if(arguments != 3){
                    char* errormessage = "Error! Unsupported parameters for command: mv";
                    write(1,errormessage,strlen(errormessage));
                }
                else{
                    moveFile(parsed_command[1],parsed_command[2]);
                }
            }
            else if (parsed_command[0] == "rm"){
                if(arguments != 2){
                    char* errormessage = "Error! Unsupported parameters for command: rm";
                    write(1,errormessage,strlen(errormessage));
                }
                else{
                    deleteFile(parsed_command[1]);
                }
            }
            else if (parsed_command[0] == "cat"){
                if(arguments != 2){
                    char* errormessage = "Error! Unsupported parameters for command: cat";
                    write(1,errormessage,strlen(errormessage));
                }
                else{
                    displayFile(parsed_command[1]);
                }
            }
            else if (parsed_command[0] ==  "EXIT"){
                exit = 1;
                break;
            }
            else{
                char error[256];
                snprintf(error, sizeof(error), "Error! Unrecognized command: ", parsed_command[0]);
                write(1,error,strlen(error));
            }
            free(parsed_command);
        }

        free_command_line(&cmd);
        if (exit == 1){break;}
    }

}
