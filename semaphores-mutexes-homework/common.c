#include <assert.h>
#include <math.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void init(unsigned int buf_size, unsigned int consumers, unsigned int producers);
void clean();
void consumer(unsigned int id);
void producer(unsigned int id, unsigned int message_id);

typedef struct {
    unsigned int id;
} thread_args_t;

unsigned int producer_max_loops;
unsigned int message_id_offset;

void *consumer_proc(void *args)
{
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

    unsigned int id = ((thread_args_t *)args)->id;

    while (true) {
        consumer(id);
    }

    return NULL;
}

void *producer_proc(void *args)
{
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

    unsigned int id = ((thread_args_t *)args)->id;

    for (unsigned int i = 0; i < producer_max_loops; i++) {
        unsigned int message_id = id * message_id_offset + i;
        producer(id, message_id);
    }

    return NULL;
}

void usage(const char *name)
{
    fprintf(stderr, "Usage: %s <producers> <consumers> <buf-size>\n", name);
}

int main(int argc, char **argv)
{
    unsigned int producers;
    unsigned int consumers;
    unsigned int buf_size;

    pthread_t *prod_threads;
    pthread_t *cons_threads;
    thread_args_t *prod_thread_args;
    thread_args_t *cons_thread_args;

    if (argc != 4) {
        usage(argv[0]);

        exit(EXIT_FAILURE);
    }

    if (1 != sscanf(argv[1], "%u", &producers)
        || 1 != sscanf(argv[2], "%u", &consumers)
        || 1 != sscanf(argv[3], "%u", &buf_size)) {
        usage(argv[0]);

        exit(EXIT_FAILURE);
    }

    assert(producers > 0);
    assert(consumers > 0);
    assert(buf_size > 0);

    producer_max_loops = 5;
    message_id_offset = pow(10, (int)ceil(log10(producer_max_loops)));

    prod_threads = malloc(sizeof(pthread_t) * producers);
    cons_threads = malloc(sizeof(pthread_t) * consumers);
    prod_thread_args = malloc(sizeof(thread_args_t) * producers);
    cons_thread_args = malloc(sizeof(thread_args_t) * consumers);

    init(buf_size, consumers, producers);

    for (unsigned int i = 0; i < producers; i++) {
        prod_thread_args[i].id = i;

        if (0 != pthread_create(&prod_threads[i], NULL, producer_proc, &prod_thread_args[i])) {
            perror("Unable to create a producer thread");

            exit(EXIT_FAILURE);
        }
    }
    for (unsigned int i = 0; i < consumers; i++) {
        cons_thread_args[i].id = i;

        if (0 != pthread_create(&cons_threads[i], NULL, consumer_proc, &cons_thread_args[i])) {
            perror("Unable to create a consumer thread");

            exit(EXIT_FAILURE);
        }
    }

    for (unsigned int i = 0; i < producers; i++) {
        pthread_join(prod_threads[i], NULL);
    }

    printf("Producers ended, waiting for consumers.\n");

    // Here I should check if the buffer is empty instead of sleeping, but I
    // don't want to excessivly complexify my code.
    sleep(1);

    for (unsigned int i = 0; i < consumers; i++) {
        pthread_cancel(cons_threads[i]);
        pthread_join(cons_threads[i], NULL);
    }

    clean();

    free(prod_threads);
    free(cons_threads);

    exit(EXIT_SUCCESS);
}
