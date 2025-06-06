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
#include "simulation_stats.h"


int main(int argc, char *argv[]) {
    int option;
    while ((option = getopt(argc, argv, ":n:c:p:w:r:h")) != -1) {
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
            case 'h':
                printf("USAGE:\n");
                printf("  ./park [OPTIONS]\n\n");
                printf("OPTIONS:\n");
                printf("  -n INT    Number of passenger threads\n");
                printf("  -c INT    Number of cars\n");
                printf("  -p INT    Capacity per car\n");
                printf("  -w INT    Car waiting period in seconds\n");
                printf("  -r INT    Ride duration in seconds\n");
                printf("  -h        Display this help message\n");
                exit(0); 
            case ':':
                fprintf(stderr, "Option -%c requires a value\n", optopt);
                break;
            case '?':
                fprintf(stderr, "Unknown option: -%c\n", optopt);
                break;
        }
    }
    for(; optind < argc; optind++) {
        printf("Given extra arguments: %s\n", argv[optind]);
        exit(EXIT_FAILURE);
    }

    printf("\n----- DUCK PARK SIMULATION -----\n");
    printf("[Monitor] Simulation started with parameters:\n");
    printf("- Number of passenger threads: %d\n", num_passengers);
    printf("- Number of cars: %d\n", num_cars);
    printf("- Capacity per car: %d\n", loading_capacity);
    printf("- Park Exploration Time: 2-5 seconds\n");
    printf("- Car waiting period: %d\n", waiting_time);
    printf("- Ride duration: %d seconds\n\n", duration);

    int pipe_fd[2];
    if (pipe(pipe_fd) == -1) {
        perror("pipe failed");
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork();

    if (pid < 0) {
        perror("fork failed");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        close(pipe_fd[1]);  

        char buffer[8192];
        ssize_t bytes_read;
        while ((bytes_read = read(pipe_fd[0], buffer, sizeof(buffer) - 1)) > 0) {
            buffer[bytes_read] = '\0';        
            fputs(buffer, stdout);            
            fflush(stdout);                   
        }
        close(pipe_fd[0]);
        exit(0);
    } else {
        close(pipe_fd[0]); 

        start_timer();
        initializer();
        create_passenger_threads(num_passengers);
        create_car_threads(num_cars);
        initialize_monitor(pipe_fd[1]);

        sleep(60);

        for (int i = 0; i < num_passengers; ++i) pthread_cancel(passenger_id_array[i]);
        for (int i = 0; i < num_cars; ++i) pthread_cancel(car_id_array[i]);
        pthread_cancel(monitor_thread);

        for (int i = 0; i < num_passengers; ++i) pthread_join(passenger_id_array[i], NULL);
        for (int i = 0; i < num_cars; ++i) pthread_join(car_id_array[i], NULL);
        pthread_join(monitor_thread, NULL);

        print_elapsed_time();
        printf("all threads have exited park\n");
        print_final_stats(pipe_fd[1]);

        close(pipe_fd[1]);
        wait(NULL); 
        cleanup();
    }
    return 0;
}
