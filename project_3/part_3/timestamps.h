#ifndef TIME_STAMPS_H
#define TIME_STAMPS_H

#include <stdio.h>
#include <time.h>

static time_t start_time;

void start_timer();
void print_elapsed_time();
void get_elapsed_time_string(char* buffer, size_t size);

#endif