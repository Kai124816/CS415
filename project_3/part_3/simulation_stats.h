#ifndef SIMULATION_STATS_H
#define SIMULATION_STATS_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include "car_state.h"
#include "queue.h"

typedef struct {
    float total_wait;
    int num_waits;
} waitime;

typedef struct {
    float total_util;
    int num_rides;
} car_util;

void initialize_waittime(waitime* w1);
void initialzie_car_util(car_util* c1);
float average_wait_time(waitime* w1);
float average_utilization(car_util* c1);
char* car_state_buffer(Car* c1, int car_id);
char* passenger_state_buffer(Car** car_array, Queue* q1, Queue* q2, int in_park, int num_cars);
char* queue_buffer(Queue* q);

#endif