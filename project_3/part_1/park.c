#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include "threads.h"
#include "car_state.h"
#include "queue.h"
#include "passenger_cond.h"
#include "timestamps.h"

int main(int argc, char *argv[]) {
    int option;

    while ((option = getopt(argc, argv, ":n:c:p:w:r:")) != -1) {
        switch (option) {
            case 'n':
                num_passengers = atoi(optarg);
                break;
            case 'c':
                num_cars = atoi(optarg);
                break;
            case 'p':
                loading_capacity = atoi(optarg);
                break;
            case 'w':
                waiting_time = atoi(optarg);
                break;
            case 'r': 
                duration = atoi(optarg);
                break;
            case ':':
                fprintf(stderr, "Option -%c requires a value\n", optopt);
                break;
            case '?':
                fprintf(stderr, "Unknown option: -%c\n", optopt);
                break;
        }
    }
    for(; optind < argc; optind++){ //when some extra arguments are passed
        printf("Given extra arguments: %s\n", argv[optind]);
        exit(EXIT_FAILURE);
    }

    printf("\n");
    printf("----- DUCK PARK SIMULATION -----\n");
    printf("[Monitor] Simulation started with parameters:\n");
    printf("- Number of passenger threads: %d\n",num_passengers);
    printf("- Number of cars: %d\n",num_cars);
    printf("- Capacity per car: %d\n",loading_capacity);
    printf("- Park Exploration Time: 2-5 seconds\n");
    printf("- Car waiting period: %d\n",waiting_time);
    printf("- Ride duration: %d seconds\n",duration);
    printf("\n");

    start_timer();
    initializer();
    create_passenger_threads(num_passengers);
    create_car_threads(num_cars);
    sleep(60);

    for(int i = 0; i < num_passengers; ++i)
    {
        pthread_cancel(passenger_id_array[i]);
    }
    for(int i = 0; i < num_cars; ++i)
    {
        pthread_cancel(car_id_array[i]);
    }
    for (int i = 0; i < num_passengers; ++i)
    {
		pthread_join(passenger_id_array[i], NULL);			
	}
    for (int i = 0; i < num_cars; ++i)
    {
		pthread_join(car_id_array[i], NULL);			
	}
    printf("all threads have exited park\n");
    cleanup();
}