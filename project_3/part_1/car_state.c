#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include "car_state.h"
#include "queue.h"


void intializeCar(Car* c1,int capacity)
{
    c1->passengers = (Queue*)malloc(sizeof(Queue));
    initializeQueue(c1->passengers);
    c1->loading = 0;
    c1->running = 0;
    c1->capacity = capacity;
}


void create_car_array(Car** c2, int num_cars,int capacity)
{
    for(int i=0; i<num_cars; i++)
    {
        c2[i] = (Car*)malloc(sizeof(Car));
        intializeCar(c2[i],capacity);
    }
}


void free_car_array(Car** c2, int num_cars)
{
    for(int i=0; i<num_cars; i++)
    {
        free(c2[i]->passengers);
        free(c2[i]);
    }
    free(c2);
}


