#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>

struct thread_args {
    sem_t *semaphores;
    int loops;
    int nb_threads;
    int id;
};

void *thread_func(void *args)
{
    struct thread_args targs = *(struct thread_args *)args;

    for (int i = 0; i < targs.loops; i++) {
        // TODO: handle EINTR
        sem_wait(&targs.semaphores[targs.id]);
        printf("I am Process %d\n", targs.id);
        sem_post(&targs.semaphores[(targs.id + 1) % targs.nb_threads]);
    }

    return NULL;
}

int main(int argc, char **argv)
{
    const int loops = 10;
    const int nb_threads = 10;

    sem_t semaphores[nb_threads];
    pthread_t threads[nb_threads];
    struct thread_args thread_args[nb_threads];

    for (int i = 0; i < nb_threads; i++) {
        thread_args[i].semaphores = semaphores;
        thread_args[i].loops = loops;
        thread_args[i].nb_threads = nb_threads;
        thread_args[i].id = i;

        if (0 != sem_init(&semaphores[i], 0, i == 0 ? 1 : 0)) {
            perror("Unable to create a semaphore");
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < nb_threads; i++) {
        if (0 != pthread_create(&threads[i], NULL, thread_func, &thread_args[i])) {
            perror("Unable to create a thread");
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < nb_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    for (int i = 0; i < nb_threads; i++) {
        sem_destroy(&semaphores[i]);
    }

    exit(EXIT_SUCCESS);
}
