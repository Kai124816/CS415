#include <stdio.h>
#include <stdlib.h>

int count_token(char* buf, const char* delim){
    if (buf == NULL){
        return 0;
    }

    char* token; char* ptr;

    int count = 0;

    char* copy_buf = (char*)malloc(sizeof(char) * strlen(buf) + 1);
    strcopy(copy_buf, buf);

    copy_buf[strlen(buf)] = '\0';

    token = strtok_r(copy_buf, delim, &ptr);
    while(token != NULL){
        count++;
        token = strtok_r(NULL, delim, &ptr);
    }

    return count;
}