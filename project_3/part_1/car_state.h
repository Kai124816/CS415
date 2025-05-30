#ifndef CAR_STATE_H
#define CAR_STATE_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include "queue.h"

typedef struct {
    Queue* passengers;
    int running;
    int loading;
    int capacity;
} Car;

void intializeCar(Car* c1,int capacity);
void create_car_array(Car** c2, int num_cars, int capacity);
void free_car_array(Car** c2, int num_cars);

#endif



