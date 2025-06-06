#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include "simulation_stats.h"
#include "queue.h"

void initialize_waittime(waitime* w1)
{
    w1->total_wait = 0.0f;
    w1->num_waits = 0;
}


void initialzie_car_util(car_util* c1)
{
    c1->total_util = 0.0f;
    c1->num_rides = 0;
}


float average_wait_time(waitime* w1)
{
    return w1->total_wait/w1->num_waits;
}


float average_utilization(car_util* c1)
{
    return c1->total_util/c1->num_rides;
}


char* car_state_buffer(Car* c1, int car_id) {
    char* buffer = malloc(64);

    int in_car = c1->passengers->count;
    if (c1->running) {
        snprintf(buffer, 64, "Car %d Status: RUNNING (%d/%d passengers)\n",
                 car_id, in_car, c1->capacity+in_car);
    } else if (c1->loading) {
        snprintf(buffer, 64, "Car %d Status: LOADING (%d/%d passengers)\n",
                 car_id, in_car, c1->capacity+in_car);
    } else {
        snprintf(buffer, 64, "Car %d Status: WAITING (0/%d passengers)\n",
                 car_id, c1->capacity);
    }

    return buffer;
}


char* passenger_state_buffer(Car** car_array, Queue* q1, Queue* q2, int in_park, int num_cars)
{
    char* buffer = malloc(128);
    int on_rides = 0;
    int exploring = 0;
    int in_queues = q1->count + q2->count;

    for(int i = 0; i<num_cars; i++)
    {
        if (car_array[i]->loading || car_array[i]->running)
        {
            on_rides += car_array[i]->passengers->count;
        }
    }

    exploring = in_park - on_rides - in_queues;
    snprintf(buffer, 128, "Passengers in park: %d (%d exploring, %d in queues, %d on rides)\n", in_park, exploring, in_queues, on_rides);
    return buffer;
}


char* queue_buffer(Queue* q) {
    char* buffer = malloc(1024);

    buffer[0] = '\0'; 

    if (isEmpty(q)) {
        strcpy(buffer, "[]\n");
        return buffer;
    }

    strcat(buffer, "[");
    for (int i = q->front + 1; i < q->rear; i++) {
        char temp[32];
        sprintf(temp, "Passenger %d", q->items[i]);
        strcat(buffer, temp);
        if (i < q->rear - 1)
            strcat(buffer, ", ");
    }
    strcat(buffer, "]\n");

    return buffer; 
}