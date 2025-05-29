#include <stdio.h>
#include <time.h>
#include "timestamps.h"

void start_timer() {
    start_time = time(NULL);
}

void print_elapsed_time() {
    time_t current_time = time(NULL);
    time_t elapsed = current_time - start_time;

    int hours = (int)(elapsed / 3600);
    int minutes = (int)((elapsed % 3600) / 60);
    int seconds = (int)(elapsed % 60);

    printf("[Time: %02d:%02d:%02d] ", hours, minutes, seconds);
}
