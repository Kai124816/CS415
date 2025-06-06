#ifndef THREADS_H
#define THREADS_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include "queue.h"
#include "car_state.h"
#include "passenger_cond.h"
#include "simulation_stats.h"

//Global Variables and Structs
extern int ready_to_load;
extern int loading_capacity;
extern int num_cars;
extern int duration;
extern int num_passengers;
extern int waiting_time;
extern int in_park;
extern int total_rides;
extern int in_park;
extern int* rounds_array;
extern int* ready_to_unboard;
extern Queue* ticket_queue;
extern Queue* ride_queue;
extern Queue* car_queue;
extern Car** car_array;
extern passenger_cond** cond_array;
extern pthread_t* passenger_id_array;
extern pthread_t* car_id_array;
extern pthread_t monitor_thread;
extern waitime* ticket_wait;
extern waitime* car_wait;
extern car_util* util;
extern pthread_mutex_t print_lock;
extern pthread_mutex_t ticket_line_lock;
extern pthread_mutex_t car_queue_lock;
extern pthread_mutex_t increment_lock;
extern sem_t sem;

//Thread Functions
void create_passenger_threads(int num_threads);
void* passenger_routine(void* arg);
void explore_park(int id);
void wait_for_ticket(int id);
void wait_for_ride(int id);
void ride_car(int id);

//Car Functions
void create_car_threads(int num_cars);
void* car_routine(void* arg);
void wait_car(int id);
void load(int id);
void ride(int id);
void unload(int id);

//Monitor Thread
void initialize_monitor(int pipe_fd);
void* monitor_routine(void* arg);
void print_simulation_state(int pipe_fd);
void print_final_stats(int pipe_fd);

//Helper Functions 
void initializer();
void cleanup();


#endif