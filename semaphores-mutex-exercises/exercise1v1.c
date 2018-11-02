#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>
#include <pthread.h>

const int loops = 10;
sem_t sem_a;
sem_t sem_b;

void *thread_a_func(void *args)
{
    for (int i = 0; i < loops; i++) {
        sem_wait(&sem_a);
        printf("I am Process 0\n");
        sem_post(&sem_b);
    }

    return NULL;
}

void *thread_b_func(void *args)
{
    for (int i = 0; i < loops; i++) {
        sem_wait(&sem_b);
        printf("I am Process 1\n");
        sem_post(&sem_a);
    }

    return NULL;
}

int main(int argc, char **argv)
{
    pthread_t thread_a;
    pthread_t thread_b;

    if (0 != sem_init(&sem_a, 0, 1)) {
        perror("Unable to create the a semaphore");
        exit(EXIT_FAILURE);
    }

    if (0 != sem_init(&sem_b, 0, 0)) {
        perror("Unable to create the b semaphore");
        exit(EXIT_FAILURE);
    }

    if (0 != pthread_create(&thread_a, NULL, (void * (*)(void *))&thread_a_func, NULL)) {
        perror("Unable to create the a thread");
        exit(EXIT_FAILURE);
    }

    if (0 != pthread_create(&thread_b, NULL, (void * (*)(void *))&thread_b_func, NULL)) {
        perror("Unable to create the b thread");
        exit(EXIT_FAILURE);
    }

    pthread_join(thread_a, NULL);
    pthread_join(thread_b, NULL);
    sem_destroy(&sem_b);
    sem_destroy(&sem_a);

    exit(EXIT_SUCCESS);
}
