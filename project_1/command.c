#include <stdio.h>
#include <dirent.h>
#include <string.h>   
#include <unistd.h>    
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <libgen.h>
#include <command.h>

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
    
    if (cwd == NULL) {
        perror("getcwd");
        return EXIT_FAILURE;
    }

    write(1,cwd,strlen(cwd));
    
}


void makeDir(char *dirName){
    int result = mkdir(dirName, 0777);

    if(result = 1){
        write(1,"error creating directory",strlen("error creating directory"));
    }
}


void changeDir(char *dirName){
    int result = chdir(dirName);

    if (result != 0){
        write(1,"cd: no such file or directory",strlen("cd: no such file or directory"));
    }
}


void copyFile(char *sourcePath, char *destinationPath){
    struct stat path_stat;
    stat(destinationPath,&path_stat);
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
        write(1,"No such file or directory",strlen("No such file or directory"));
    }
    else{
        remove(filename);
    }
}


void displayFile(char *filename){
    int src = open("input.txt", O_RDONLY);
    char buffer[4096];
    ssize_t bytes;

    while ((bytes = read(src, buffer, sizeof(buffer))) > 0) {
        write(1, buffer, bytes);
    }
}