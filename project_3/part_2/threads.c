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
int ready_to_load = 0;
int loading_capacity = 1;
int num_cars = 1;
int num_passengers = 1;
int waiting_time = 10;
int duration = 10;
int in_park = 0;
int* rounds_array = NULL;
int* ready_to_unboard = NULL;
Queue* ticket_queue = NULL;
Queue* ride_queue = NULL;
Queue* car_queue = NULL;
Car** car_array = NULL;
passenger_cond** cond_array = NULL;
pthread_t* passenger_id_array = NULL;
pthread_t* car_id_array = NULL;
pthread_mutex_t ticket_line_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t car_queue_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t increment_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t print_lock = PTHREAD_MUTEX_INITIALIZER;
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

    while(peek(ticket_queue) != id){sleep(1);}
    pthread_mutex_lock(&print_lock);
    print_elapsed_time();
    printf("Passenger %d is acquiring ticket\n",id);
    pthread_mutex_unlock(&print_lock);
    sleep(2);

    pthread_mutex_lock(&print_lock);
    print_elapsed_time();
    printf("Passenger %d acquired ticket\n",id);
    pthread_mutex_unlock(&print_lock);
    sem_wait(&sem);
    enqueue(ride_queue,id);
    dequeue(ticket_queue);
    pthread_mutex_lock(&print_lock);
    print_elapsed_time();
    printf("Passenger %d joined the ride queue\n",id);
    pthread_mutex_unlock(&print_lock);
}


void wait_for_ride(int id)
{
    pthread_mutex_lock(&cond_array[id]->board_lock);
    while (!ready_to_load) {
        pthread_cond_wait(&cond_array[id]->board_cond, &cond_array[id]->board_lock);
    }
    pthread_mutex_unlock(&cond_array[id]->board_lock);
    ready_to_load = 0;
    sem_post(&sem);
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
    dequeue(car_queue);
}


void ride(int id)
{   
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
    print_elapsed_time();
    printf("car %d is unloading\n",id);
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
    initializeQueue(ticket_queue);
    initializeQueue(ride_queue);
    initializeQueue(car_queue);
    create_car_array(car_array,num_cars,loading_capacity);
    create_cond_array(cond_array,num_passengers);
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
    free_car_array(car_array, num_cars);
    free_cond_array(cond_array,num_passengers);
    pthread_mutex_destroy(&ticket_line_lock);
    pthread_mutex_destroy(&car_queue_lock);
    pthread_mutex_destroy(&increment_lock);
    pthread_mutex_destroy(&print_lock);
    sem_destroy(&sem);
}


