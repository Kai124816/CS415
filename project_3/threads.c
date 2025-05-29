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

//variable definitions
int stop = 0;
int ready_to_load = 0;
int ready_to_unboard = 0;
int loading_capacity = 1;
int num_cars = 1;
int num_passengers = 1;
int waiting_time = 10;
int duration = 10;
int* rounds_array = NULL;
Queue* ticket_queue = NULL;
Queue* ride_queue = NULL;
Queue* car_queue = NULL;
Car** car_array = NULL;
passenger_cond** cond_array = NULL;
pthread_mutex_t ticket_line_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t car_queue_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t increment_lock = PTHREAD_MUTEX_INITIALIZER;

void printArray(int arr[], int size) {
    for (int i = 0; i < size; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
}


//Passenger Functionality
void create_passenger_threads(int num_threads)
{
    pthread_t* thread_array = (pthread_t*)malloc(num_threads * sizeof(pthread_t));

    for (int i = 0; i < num_threads; i++) 
    {
        int* tid = malloc(sizeof(int));
        *tid = i;   
        pthread_create(&thread_array[i], NULL, passenger_routine, tid);
    }

    free(thread_array);
}


void* passenger_routine(void* arg)
{
    int *thread_id = (int*)arg;
    sleep(*thread_id);
    print_elapsed_time();
    printf("Passenger %d has entered the park\n",*thread_id);

    while(!stop)
    {
        explore_park(*thread_id);
        wait_for_ticket(*thread_id);
        wait_for_ride(*thread_id);
        ride_car(*thread_id);
        rounds_array[*thread_id]++;
    }

    free(arg);
    return NULL;
}


void explore_park(int id)
{
    print_elapsed_time();
    printf("Passenger %d is exploring_park\n",id);
    int exploration_time = (rand() % 4) + 2;
    sleep(exploration_time);
    pthread_mutex_lock(&ticket_line_lock);
    enqueue(ticket_queue,id);
    pthread_mutex_unlock(&ticket_line_lock);
    print_elapsed_time();
    printf("Passenger %d is getting in the ticket line\n",id);
    if(stop){pthread_exit(NULL);}
}


void wait_for_ticket(int id)
{
    print_elapsed_time();
    printf("Passenger %d waiting in line for ticket\n",id);
    while(peek(ticket_queue) != id && !stop){sleep(1);}
    print_elapsed_time();
    printf("Passenger %d is acquiring ticket\n",id);
    sleep(2);
    print_elapsed_time();
    printf("Passenger %d acquired ticket\n",id);
    enqueue(ride_queue,id);
    dequeue(ticket_queue);
    print_elapsed_time();
    printf("Passenger %d joined the ride queue\n",id);
    if(stop){pthread_exit(NULL);}
}


void wait_for_ride(int id)
{
    pthread_mutex_lock(&cond_array[id]->board_lock);
    while (!ready_to_load && !stop) {
        pthread_cond_wait(&cond_array[id]->board_cond, &cond_array[id]->board_lock);
    }
    pthread_mutex_unlock(&cond_array[id]->board_lock);
    ready_to_load = 0;
    if(stop){pthread_exit(NULL);}
}


void ride_car(int id)
{
    pthread_mutex_lock(&cond_array[id]->unboard_lock);
    while (!ready_to_unboard && !stop){
        pthread_cond_wait(&cond_array[id]->unboard_cond, &cond_array[id]->unboard_lock);
    }
    pthread_mutex_unlock(&cond_array[id]->unboard_lock);
    ready_to_unboard = 0;
    if(stop){pthread_exit(NULL);}
}


//Car Functionality
void create_car_threads(int num_cars)
{
    pthread_t* thread_array = malloc(num_cars * sizeof(pthread_t));

    for (int i = 0; i < num_cars; i++) 
    {
        int* tid = malloc(sizeof(int));
        *tid = i;   
        pthread_create(&thread_array[i], NULL, car_routine, tid);
    }

    free(thread_array);
}


void* car_routine(void* arg)
{
    int *thread_id = (int*)arg;	

    while(1)
    {
        wait_car(*thread_id);
        load(*thread_id);
        ride(*thread_id);
        unload(*thread_id);
    }

    free(arg);
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
    print_elapsed_time();
    printf("car %d invoked load()\n", id);
    car_array[id]->loading = 1;
    clock_t start = clock();
    double cpu_time_used = 0.0;

    while(car_array[id]->capacity > 0 && cpu_time_used < waiting_time && !stop)
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
            print_elapsed_time();
            printf("Passenger %d boarded car %d\n",p1,id);
        }
        else{sleep(1);}
        clock_t end = clock(); // Record end time
        cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    }
    if(stop){pthread_exit(NULL);}
    dequeue(car_queue);
}


void ride(int id)
{   
    car_array[id]->loading = 0;
    car_array[id]->running = 1;
    print_elapsed_time();
    printf("car %d is running\n",id);
    sleep(duration);
    if(stop){pthread_exit(NULL);}
}


void unload(int id)
{
    print_elapsed_time();
    printf("car %d is unloading\n",id);
    while(peek(car_array[id]->passengers) != -1 && !stop)
    {
        int p1 = dequeue(car_array[id]->passengers);
        pthread_mutex_lock(&cond_array[p1]->unboard_lock);
        ready_to_unboard = 1; 
        pthread_cond_signal(&cond_array[p1]->unboard_cond); 
        pthread_mutex_unlock(&cond_array[p1]->unboard_lock);
        print_elapsed_time();
        printf("Passenger %d unboarded car %d\n",p1,id);
    }
    if(stop){pthread_exit(NULL);}
    car_array[id]->capacity = loading_capacity;
    car_array[id]->loading = 0;
    car_array[id]->running = 0;
}


//Helper Functions
void initializer()
{
    rounds_array = (int*)calloc(num_passengers,sizeof(int));
    ticket_queue = (Queue*)malloc(sizeof(Queue));
    ride_queue = (Queue*)malloc(sizeof(Queue));
    car_queue = (Queue*)malloc(sizeof(Queue));
    car_array = (Car**)malloc(sizeof(Car*)*num_cars);
    cond_array = (passenger_cond**)malloc(sizeof(passenger_cond*)*num_passengers);
    initializeQueue(ticket_queue);
    initializeQueue(ride_queue);
    initializeQueue(car_queue);
    create_car_array(car_array,num_cars,loading_capacity);
    create_cond_array(cond_array,num_passengers);
}


int check_to_stop()
{
    int min_rounds = 2;
    for(int i=0; i<num_passengers; i++)
    {
        if(rounds_array[i] < min_rounds)
        {
            return 0;
        }
    }
    return 1;
}


void cleanup()
{
    free(rounds_array);
    free(ticket_queue);
    free(ride_queue);
    free(car_queue);
    free_car_array(car_array, num_cars);
    free_cond_array(cond_array,num_passengers);
}



