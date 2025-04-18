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
        write(1,entry->d_name,strlen(entry->d_name));
    }
}


void showCurrentDir(){
    char* buffer;
    char *cwd = getcwd(buffer,256);
    write(1,cwd,strlen(cwd)); 
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
        snprintf(error, sizeof(error), "cp: cannot stat '%s\n': No such file or directory", sourcePath);
        write(1,error,strlen(error));
        return;
    }

    struct stat path_stat;
    int result = stat(destinationPath,&path_stat);
    if (result == -1){
        char error[256];
        snprintf(error, sizeof(error), "cp: cannot create regular file '%s\n': No such file or directory", destinationPath);
        write(1,error,strlen(error));
    }
    char* filename = basename(sourcePath);
    int dest;

    if (S_ISDIR(path_stat.st_mode)){
        char* buffer = (char*)malloc(strlen(filename) + strlen(destinationPath) + 1);
        buffer = strcat(destinationPath,filename);
        dest = open(buffer, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    }
    else{
        dest = open(destinationPath, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    }
    
    int src = open(sourcePath, O_RDONLY);
    char buffer[4096];
    ssize_t bytes;
    while ((bytes = read(src, buffer, sizeof(buffer))) > 0) {
        write(dest, buffer, bytes);
    }

    close(src);
    close(dest);
    if (S_ISDIR(path_stat.st_mode)){free(buffer);}    
}


void moveFile(char *sourcePath, char *destinationPath){
    struct stat checker1;
    if(stat(sourcePath, &checker1) != 0){
        char error[256];
        snprintf(error, sizeof(error), "mv: cannot stat '%s\n': No such file or directory", sourcePath);
        write(1,error,strlen(error));
        return;
    }
    struct stat checker2;
    if(stat(destinationPath, &checker2) != 0){
        char error[256];
        snprintf(error, sizeof(error), "mv: cannot stat '%s\n': No such file or directory", destinationPath);
        write(1,error,strlen(error));
        return;
    }

    copyFile(sourcePath,destinationPath);
    deleteFile(sourcePath);
}


void deleteFile(char *filename){
    DIR *current = opendir("."); 
    struct dirent *entry;
    char* file = basename(filename);
    int found = 0;

    while ((entry = readdir(current)) != NULL) {
        if(strcmp(entry->d_name,file) == 0){
            found = 1;
            break;
        }
    }

    if(found == 0){
        char error[256];
        snprintf(error, sizeof(error), "rm: cannot remove ‘%s\n’: No such file or directory", filename);
        write(1,error,strlen(error));
    }
    else{
        remove(filename);
    }
}


void displayFile(char *filename){
    struct stat checker;
    if(stat(filename, &checker) != 0){
        char error[256];
        snprintf(error, sizeof(error), "cat: '%s\n': No such file or directory", filename);
        write(1,error,strlen(error));
        return;
    }

    int src = open("input.txt", O_RDONLY);
    char buffer[4096];
    ssize_t bytes;

    while ((bytes = read(src, buffer, sizeof(buffer))) > 0) {
        write(1, buffer, bytes);
    }
}