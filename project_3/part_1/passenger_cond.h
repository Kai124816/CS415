#ifndef PASSENGER_COND_H
#define PASSENGER_COND_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

typedef struct {
    pthread_mutex_t board_lock;
    pthread_cond_t board_cond;
    pthread_mutex_t unboard_lock;
    pthread_cond_t unboard_cond;
} passenger_cond;

void create_cond_array(passenger_cond** c1, int num_passengers);
void free_cond_array(passenger_cond** c1, int num_passengers);

#endif
