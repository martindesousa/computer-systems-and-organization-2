#include <stdio.h>
#include <stdlib.h> 
#include <pthread.h>
#include <unistd.h>

pthread_barrier_t barrier;

pthread_t philosopher[5];
pthread_mutex_t chopstick[5];
int backoff_time = 50;
int random_time = 0;

void *eat(void *arg) {
    int n = (int) (long)arg;

    // Sync up threads at the start to make deadlock more consistent
    pthread_barrier_wait(&barrier);

    while (1) {
        pthread_mutex_lock(&chopstick[n]);
        printf("Philosopher %d got chopstick %d\n", n, n);

        
        if (pthread_mutex_trylock(&chopstick[(n+1)%5]) == 0) {
            printf("Philosopher %d got chopstick %d\n", n, (n+1)%5);
            printf("Philosopher %d is eating\n", n);

            
            printf("Philosopher %d set down chopstick %d\n", n, (n+1)%5);
            pthread_mutex_unlock(&chopstick[(n+1)%5]);
            printf("Philosopher %d set down chopstick %d\n", n, n);
            pthread_mutex_unlock(&chopstick[n]);

            return NULL;  
        } else {  // Failed to get the second chopstick
            printf("Philosopher %d set down chopstick %d\n", n, n);
            pthread_mutex_unlock(&chopstick[n]);

            random_time = backoff_time + (rand() % backoff_time);
            usleep(random_time * 1000);  
            backoff_time *= 2;  
        }
    }
}

int main(int argc, const char *argv[]) {
    pthread_barrier_init(&barrier, NULL, 5);

    for(int i = 0; i < 5; i++)
        pthread_mutex_init(&chopstick[i], NULL);

    for(int i = 0; i < 5; i++)
        pthread_create(&philosopher[i], NULL, eat, (void *)(size_t)i);

    for(int i = 0; i < 5; i++)
        pthread_join(philosopher[i], NULL);

    for(int i = 0; i < 5; i++)
        pthread_mutex_destroy(&chopstick[i]);

    pthread_barrier_destroy(&barrier);

    return 0;
}
