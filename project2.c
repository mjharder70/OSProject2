#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include<time.h>

#define NUM_GUESTS 5
#define NUM_ROOMS 3

// Define semaphores
sem_t roomsAvailable;
sem_t checkinReservist;
sem_t checkoutReservist;
sem_t guestSync;

//define variables
int totalGuests = 0;
int poolCount = 0;
int restaurantCount = 0;
int fitnessCenterCount = 0;
int businessCenterCount = 0;

// start Functions
void *guestFunction(void *arg);
void *checkinFunction(void *arg);
void *checkoutFunction(void *arg);
void printSummary();

int main() {
    // Initialize semaphores
    sem_init(&roomsAvailable, 0, NUM_ROOMS);
    sem_init(&checkinReservist, 0, 1);
    sem_init(&checkoutReservist, 0, 1);
    sem_init(&guestSync, 0, 1);

    // Create threads for check-in and check-out reservationists
    pthread_t checkin_thread, checkout_thread;
    pthread_create(&checkin_thread, NULL, checkinFunction, NULL);
    pthread_create(&checkout_thread, NULL, checkoutFunction, NULL);

    // Create threads for guests
    pthread_t guests[NUM_GUESTS];
    int guest_ids[NUM_GUESTS];
    for (int i = 0; i < NUM_GUESTS; i++) {
        guest_ids[i] = i;
        pthread_create(&guests[i], NULL, guestFunction, &guest_ids[i]);
    }

    // Join threads
    pthread_join(checkin_thread, NULL);
    pthread_join(checkout_thread, NULL);
    for (int i = 0; i < NUM_GUESTS; i++) {
        pthread_join(guests[i], NULL);
    }

    //Print summary
    printSummary();

    // Destroy semaphores
    sem_destroy(&roomsAvailable);
    sem_destroy(&checkinReservist);
    sem_destroy(&checkoutReservist);
    sem_destroy(&guestSync);

    return 0;
}

//Define what the functions do

//guest behavior
void *guestFunction(void *arg) {
    int guest_id = *((int *)arg);

    //enter hotel
    printf("Guest %d enters the hotel\n", guest_id);

    //go to reception
    sem_wait(&roomsAvailable);
    printf("Guest %d goes to the check-in reservationist\n", guest_id);

    //check in
    
    sem_wait(&checkinReservist);
    printf("Guest %d receives room from the check-in\n", guest_id);
    sem_post(&checkinReservist);

    //Use hotel facilities
    srand(time(NULL));
    int activity = rand() % 4;
    switch(activity) {
        case 0:
            printf("Guest %d goes to the swimming pool\n", guest_id);
            sem_wait(&guestSync);
            poolCount++;
            sem_post(&guestSync);
            break;
        case 1:
            printf("Guest %d goes to the restaurant\n", guest_id);
            sem_wait(&guestSync);
            restaurantCount++;
            sem_post(&guestSync);
            break;
        case 2:
            printf("Guest %d goes to the fitness center\n", guest_id);
            sem_wait(&guestSync);
            fitnessCenterCount++;
            sem_post(&guestSync);
            break;
        case 3:
            printf("Guest %d goes to the business center\n", guest_id);
            sem_wait(&guestSync);
            businessCenterCount++;
            sem_post(&guestSync);
            break;
    }
    //sleep for random time 1 to 3 seconds
    sleep(rand() % 3 + 1); 

    // Check-out
     
    sem_wait(&checkoutReservist);
    printf("Guest %d checks out\n", guest_id);
    sem_post(&checkoutReservist);

    //release the romm for other guest use
    sem_post(&roomsAvailable); 

    // Increment total guests
    sem_wait(&guestSync);
    totalGuests++;
    sem_post(&guestSync);

    pthread_exit(NULL);
}

//check in behavior
void *checkinFunction(void *arg) {

        sem_wait(&checkinReservist);
        printf("The check-in reservationist greets the guest\n");
        sem_post(&checkinReservist);

}

//checkout behavior
void *checkoutFunction(void *arg) {
     
        sem_wait(&checkoutReservist);
        printf("The check-out reservationist greets the guest\n");
        sem_post(&checkoutReservist);

}

//print summary
void printSummary(){
    printf("\nAccounting summary:\n");
    printf("Total guests: %d\n", totalGuests);
    printf("Pool use: %d\n", poolCount);
    printf("Restaurant use: %d\n", restaurantCount);
    printf("Fitness center use: %d\n", fitnessCenterCount);
    printf("Business center use: %d\n", businessCenterCount);
}