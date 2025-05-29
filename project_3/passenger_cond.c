#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include "passenger_cond.h"

void create_cond_array(passenger_cond** c1, int num_passengers)
{
    for(int i=0;i<num_passengers;i++)
    { 
        c1[i] = (passenger_cond*)malloc(sizeof(passenger_cond));
        pthread_mutex_init(&c1[i]->board_lock,NULL);
        pthread_cond_init(&c1[i]->board_cond,NULL);
        pthread_mutex_init(&c1[i]->unboard_lock,NULL);
        pthread_cond_init(&c1[i]->unboard_cond,NULL);
    }
}


void free_cond_array(passenger_cond** c1, int num_passengers)
{
    for(int i=0;i<num_passengers;i++)
    {
        free(c1[i]);
    }
    free(c1);
}
