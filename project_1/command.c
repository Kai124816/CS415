#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>   
#include <unistd.h>    
#include <fcntl.h>
#include <sys/stat.h>
#include <libgen.h>
#include "command.h"


void listDir() {
    DIR *current = opendir(".");
    struct dirent *entry;

    while ((entry = readdir(current)) != NULL) {
        char name[257];
        snprintf(name, sizeof(name), "%s ", entry->d_name);
        write(1,name,strlen(name));
    }
    write(1, "\n", 1);
    closedir(current);
}


void showCurrentDir(){
    char *cwd = getcwd(NULL, 0);
    if (cwd != NULL) {
        write(1, cwd, strlen(cwd));
        write(1, "\n", 1);
        free(cwd);  
    }
}


void makeDir(char *dirName){
    int result = mkdir(dirName, 0777);

    if(result == 1){
        char error[256];
        snprintf(error, sizeof(error), "mkdir: cannot create directory '%s': File exists\n", dirName);
        write(1,error,strlen(error));
    }
}


void changeDir(char *dirName){
    int result = chdir(dirName);

    if (result != 0){
        char error[256];
        snprintf(error, sizeof(error), "bash: cd: %s: No such file or directory\n", dirName);
        write(1,error,strlen(error));
    }
}


void copyFile(char *sourcePath, char *destinationPath){
    struct stat checker;
    if(stat(sourcePath, &checker) != 0){
        char error[256];
        snprintf(error, sizeof(error), "cp: cannot stat '%s': No such file or directory\n", sourcePath);
        write(1,error,strlen(error));
        return;
    }

    struct stat path_stat;
    int result = stat(destinationPath,&path_stat);
    char* filename = basename(sourcePath);
    int dest;

    if (result == 0 && S_ISDIR(path_stat.st_mode)){
        char dest_path[512];
        snprintf(dest_path, sizeof(dest_path), "%s/%s", destinationPath, basename(sourcePath));
        dest = open(dest_path, O_WRONLY | O_CREAT | O_TRUNC, checker.st_mode & 0777);
    }
    else{
        dest = open(destinationPath, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    }
    
    int src = open(sourcePath, O_RDONLY);
    char buffer_2[4096];
    ssize_t bytes;
    while ((bytes = read(src, buffer_2, sizeof(buffer_2))) > 0) {
        write(dest, buffer_2, bytes);
    }

    close(src);
    close(dest);
}


void moveFile(char *sourcePath, char *destinationPath){
    struct stat checker1;
    if(stat(sourcePath, &checker1) != 0){
        char error[256];
        snprintf(error, sizeof(error), "mv: cannot stat '%s': No such file or directory\n", sourcePath);
        write(1,error,strlen(error));
        return;
    }

    if(sourcePath != destinationPath)
    {
        copyFile(sourcePath,destinationPath);
        deleteFile(sourcePath);
    }
}


void deleteFile(char *filename){
    int status = remove(filename);

    if(status == -1){
        char error[256];
        snprintf(error, sizeof(error), "rm: cannot remove ‘%s’: No such file or directory\n", filename);
        write(1,error,strlen(error));
    }
}


void displayFile(char *filename){
    struct stat checker;
    if(stat(filename, &checker) != 0){
        char error[256];
        snprintf(error, sizeof(error), "cat: '%s': No such file or directory\n", filename);
        write(1,error,strlen(error));
        return;
    }

    int src = open(filename, O_RDONLY);
    char buffer[4096];
    ssize_t bytes;

    while ((bytes = read(src, buffer, sizeof(buffer))) > 0) {
        write(1, buffer, bytes);
    }
    
    close(src);
}