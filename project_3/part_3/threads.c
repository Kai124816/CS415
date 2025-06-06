#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include "queue.h"
#include "threads.h"
#include "passenger_cond.h"
#include "car_state.h"
#include "timestamps.h"
#include "simulation_stats.h"

//variable definitions
int ready_to_load = 0;
int loading_capacity = 1;
int num_cars = 1;
int num_passengers = 1;
int waiting_time = 10;
int duration = 10;
int in_park = 0;
int total_rides = 0;
int* rounds_array = NULL;
int* ready_to_unboard = NULL;
Queue* ticket_queue = NULL;
Queue* ride_queue = NULL;
Queue* car_queue = NULL;
Car** car_array = NULL;
passenger_cond** cond_array = NULL;
pthread_t* passenger_id_array = NULL;
pthread_t* car_id_array = NULL;
pthread_t monitor_thread;
waitime* car_wait = NULL;
waitime* ticket_wait = NULL;
car_util* util = NULL;
pthread_mutex_t print_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t ticket_line_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t car_queue_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t increment_lock = PTHREAD_MUTEX_INITIALIZER;
sem_t sem; 


//Passenger Functionality
void create_passenger_threads(int num_threads)
{
    passenger_id_array = (pthread_t*)malloc(num_threads * sizeof(pthread_t));

    for (int i = 0; i < num_threads; i++) 
    {
        int* tid = malloc(sizeof(int));
        *tid = i;   
        pthread_create(&passenger_id_array[i], NULL, passenger_routine, tid);
    }
}


void* passenger_routine(void* arg)
{
    int thread_id = *(int*)arg; 
    free(arg);
    sleep(thread_id);
    pthread_mutex_lock(&print_lock);
    print_elapsed_time();
    printf("Passenger %d has entered the park\n",thread_id);
    in_park++;
    pthread_mutex_unlock(&print_lock);

    while(1)
    {
        explore_park(thread_id);
        wait_for_ticket(thread_id);
        wait_for_ride(thread_id);
        ride_car(thread_id);
        rounds_array[thread_id]++;
    }

    return NULL;
}


void explore_park(int id)
{
    pthread_mutex_lock(&print_lock);
    print_elapsed_time();
    printf("Passenger %d is exploring_park\n",id);
    pthread_mutex_unlock(&print_lock);
    int exploration_time = (rand() % 4) + 2;
    sleep(exploration_time);
    pthread_mutex_lock(&ticket_line_lock);
    enqueue(ticket_queue,id);
    pthread_mutex_unlock(&ticket_line_lock);
    pthread_mutex_lock(&print_lock);
    print_elapsed_time();
    printf("Passenger %d is getting in the ticket line\n",id);
    pthread_mutex_unlock(&print_lock);
}


void wait_for_ticket(int id)
{
    pthread_mutex_lock(&print_lock);
    print_elapsed_time();
    printf("Passenger %d waiting in line for ticket\n",id);
    pthread_mutex_unlock(&print_lock);

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start); 
    while(peek(ticket_queue) != id){sleep(1);}
    pthread_mutex_lock(&print_lock);
    print_elapsed_time();
    printf("Passenger %d is acquiring ticket\n",id);
    pthread_mutex_unlock(&print_lock);
    sleep(2);
    clock_gettime(CLOCK_MONOTONIC, &end);  
    long seconds = end.tv_sec - start.tv_sec;
    double elapsed_ms = seconds * 1000.0;
    ticket_wait->total_wait += elapsed_ms/1000;
    ticket_wait->num_waits++;
    
    pthread_mutex_lock(&print_lock);
    print_elapsed_time();
    printf("Passenger %d acquired ticket\n",id);
    pthread_mutex_unlock(&print_lock);
    enqueue(ride_queue,id);
    dequeue(ticket_queue);
    pthread_mutex_lock(&print_lock);
    print_elapsed_time();
    printf("Passenger %d joined the ride queue\n",id);
    pthread_mutex_unlock(&print_lock);
}


void wait_for_ride(int id)
{
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);  // Start timestamp

    pthread_mutex_lock(&cond_array[id]->board_lock);
    while (!ready_to_load) {
        pthread_cond_wait(&cond_array[id]->board_cond, &cond_array[id]->board_lock);
    }
    pthread_mutex_unlock(&cond_array[id]->board_lock);
    ready_to_load = 0;

    clock_gettime(CLOCK_MONOTONIC, &end);  // End timestamp

    long seconds = end.tv_sec - start.tv_sec;
    double elapsed_ms = seconds * 1000.0;
    car_wait->total_wait += elapsed_ms/1000;
    car_wait->num_waits++;
}



void ride_car(int id)
{
    pthread_mutex_lock(&cond_array[id]->unboard_lock);
    while (!ready_to_unboard[id]){
        pthread_cond_wait(&cond_array[id]->unboard_cond, &cond_array[id]->unboard_lock);
    }
    pthread_mutex_unlock(&cond_array[id]->unboard_lock);
    ready_to_unboard[id] = 0;
}


//Car Functionality
void create_car_threads(int num_cars)
{
    car_id_array = malloc(num_cars * sizeof(pthread_t));

    for (int i = 0; i < num_cars; i++) 
    {
        int* tid = malloc(sizeof(int));
        *tid = i;   
        pthread_create(&car_id_array[i], NULL, car_routine, tid);
    }
}


void* car_routine(void* arg)
{
    int thread_id = *(int*)arg; 
    free(arg);
    
    while(1)
    {
        wait_car(thread_id);
        load(thread_id);
        ride(thread_id);
        unload(thread_id);
    }

    return NULL;
}


void wait_car(int id)
{
    pthread_mutex_lock(&car_queue_lock);
    enqueue(car_queue,id);
    pthread_mutex_unlock(&car_queue_lock);
    while(peek(car_queue) != id){sleep(1);}
}


void load(int id)
{
    pthread_mutex_lock(&print_lock);
    print_elapsed_time();
    printf("car %d invoked load()\n", id);
    pthread_mutex_unlock(&print_lock);
    car_array[id]->loading = 1;
    clock_t start = clock();
    double cpu_time_used = 0.0;

    while(car_array[id]->capacity > 0 && cpu_time_used < waiting_time)
    {
        if(peek(ride_queue) != -1)
        {
            int p1 = dequeue(ride_queue);
            enqueue(car_array[id]->passengers,p1);
            car_array[id]->capacity--;
            pthread_mutex_lock(&cond_array[p1]->board_lock);
            ready_to_load = 1; 
            pthread_cond_signal(&cond_array[p1]->board_cond); 
            pthread_mutex_unlock(&cond_array[p1]->board_lock);
            pthread_mutex_lock(&print_lock);
            print_elapsed_time();
            printf("Passenger %d boarded car %d\n",p1,id);
            pthread_mutex_unlock(&print_lock);
            if(in_park == 1){break;}
        }
        else{sleep(1);}
        clock_t end = clock(); // Record end time
        cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    }

    int on_ride = loading_capacity - car_array[id]->capacity;
    util->total_util += on_ride/loading_capacity;
    util->num_rides++;
    dequeue(car_queue);
}


void ride(int id)
{   
    total_rides++;
    car_array[id]->loading = 0;
    car_array[id]->running = 1;
    pthread_mutex_lock(&print_lock);
    print_elapsed_time();
    printf("car %d is running\n",id);
    pthread_mutex_unlock(&print_lock);
    sleep(duration);
}


void unload(int id)
{
    pthread_mutex_lock(&print_lock);
    print_elapsed_time();
    printf("car %d is unloading\n",id);
    pthread_mutex_unlock(&print_lock);
    while(peek(car_array[id]->passengers) != -1)
    {
        int p1 = dequeue(car_array[id]->passengers);
        pthread_mutex_lock(&cond_array[p1]->unboard_lock);
        ready_to_unboard[p1] = 1; 
        pthread_cond_signal(&cond_array[p1]->unboard_cond); 
        pthread_mutex_unlock(&cond_array[p1]->unboard_lock);
        pthread_mutex_lock(&print_lock);
        print_elapsed_time();
        printf("Passenger %d unboarded car %d\n",p1,id);
        pthread_mutex_unlock(&print_lock);
    }
    car_array[id]->capacity = loading_capacity;
    car_array[id]->loading = 0;
    car_array[id]->running = 0;
}


//Monitor Thread
void initialize_monitor(int pipe_fd) {
    int* fd_ptr = malloc(sizeof(int));
    if (fd_ptr == NULL) {
        perror("malloc failed");
        exit(EXIT_FAILURE);
    }
    *fd_ptr = pipe_fd;

    pthread_create(&monitor_thread, NULL, monitor_routine, fd_ptr);
}


void* monitor_routine(void* arg) {
    int pipe_fd = *(int*)arg;
    free(arg);

    while (1) {
        sleep(5);
        print_simulation_state(pipe_fd); 
    }

    return NULL;
}


void print_simulation_state(int pipe_fd) {
    char buffer[8192];
    buffer[0] = '\0';  

    pthread_mutex_lock(&print_lock);

    strcat(buffer, "\n[Monitor] System State at ");
    char time_buf[64];
    get_elapsed_time_string(time_buf, sizeof(time_buf));  
    strcat(buffer, time_buf);
    strcat(buffer, "\n");

    char* queue_buf = queue_buffer(ticket_queue);
    strcat(buffer, "Ticket queue: ");
    strcat(buffer, queue_buf);
    free(queue_buf);

    queue_buf = queue_buffer(ride_queue);
    strcat(buffer, "Ride queue: ");
    strcat(buffer, queue_buf);
    free(queue_buf);

    for (int i = 0; i < num_cars; i++) {
        char* car_buf = car_state_buffer(car_array[i], i);
        strcat(buffer, car_buf);
        free(car_buf);
    }

    char* passenger_buf = passenger_state_buffer(car_array, ticket_queue, ride_queue, in_park, num_cars);
    strcat(buffer, passenger_buf);
    strcat(buffer, "\n");
    free(passenger_buf);

    write(pipe_fd, buffer, strlen(buffer));

    usleep(100000);

    pthread_mutex_unlock(&print_lock);
}


void print_final_stats(int pipe_fd) {
    char buffer[1024];
    buffer[0] = '\0';

    pthread_mutex_lock(&print_lock);

    strcat(buffer, "\n[Monitor] FINAL STATISTICS:\n");
    strcat(buffer, "Total simulation time: 00:01:00\n");  // Replace with actual timer later

    char line[128];
    snprintf(line, sizeof(line), "Total passengers served: %d\n", in_park);
    strcat(buffer, line);

    snprintf(line, sizeof(line), "Total rides completed: %d\n", total_rides);
    strcat(buffer, line);

    snprintf(line, sizeof(line), "Average wait time in ticket queue: %.3f seconds\n", average_wait_time(ticket_wait));
    strcat(buffer, line);

    snprintf(line, sizeof(line), "Average wait time in ride queue: %.3f seconds\n", average_wait_time(car_wait));
    strcat(buffer, line);

    snprintf(line, sizeof(line), "Average car utilization: %.2f%%\n", average_utilization(util) * 100);
    strcat(buffer, line);

    pthread_mutex_unlock(&print_lock);

    write(pipe_fd, buffer, strlen(buffer));
}


//Helper Functions
void initializer()
{
    rounds_array = (int*)calloc(num_passengers,sizeof(int));
    ready_to_unboard = (int*)calloc(num_passengers,sizeof(int));
    ticket_queue = (Queue*)malloc(sizeof(Queue));
    ride_queue = (Queue*)malloc(sizeof(Queue));
    car_queue = (Queue*)malloc(sizeof(Queue));
    car_array = (Car**)malloc(sizeof(Car*)*num_cars);
    cond_array = (passenger_cond**)malloc(sizeof(passenger_cond*)*num_passengers);
    ticket_wait = (waitime*)malloc(sizeof(waitime));
    car_wait = (waitime*)malloc(sizeof(waitime));
    util = (car_util*)malloc(sizeof(car_util));
    initializeQueue(ticket_queue);
    initializeQueue(ride_queue);
    initializeQueue(car_queue);
    create_car_array(car_array,num_cars,loading_capacity);
    create_cond_array(cond_array,num_passengers);
    initialize_waittime(ticket_wait);
    initialize_waittime(car_wait);
    initialzie_car_util(util);
    sem_init(&sem, 0, 5); 
}


void cleanup()
{
    free(rounds_array);
    free(ready_to_unboard);
    free(ticket_queue);
    free(ride_queue);
    free(car_queue);
    free(passenger_id_array);
    free(car_id_array);
    free(ticket_wait);
    free(car_wait);
    free(util);
    free_car_array(car_array, num_cars);
    free_cond_array(cond_array,num_passengers);
    pthread_mutex_destroy(&ticket_line_lock);
    pthread_mutex_destroy(&car_queue_lock);
    pthread_mutex_destroy(&increment_lock);
    sem_destroy(&sem);
}



