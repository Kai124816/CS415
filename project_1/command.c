#include <stdio.h>
#include <dirent.h>
#include <string.h>    // for strlen
#include <unistd.h>    // for write
#include <stdlib.h>

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
        perror("error creating directory");
    }
}


void changeDir(char *dirName){
    int result = chdir(dirName);

    if (result != 0){
        write(1,"cd: no such file or directory",30);
    }
}


void copyFile(char *sourcePath, char *destinationPath){
    
}

