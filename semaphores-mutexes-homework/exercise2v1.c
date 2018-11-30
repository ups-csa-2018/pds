#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>

typedef unsigned int rbuf_message_t;
#include "rbuf.h"

rbuf_t *buf;
sem_t slots_available, messages_available;
pthread_mutex_t buf_mtx;

void init(unsigned int buf_size) {
    buf = rbuf_create(buf_size);
    sem_init(&slots_available, 0, buf_size);
    sem_init(&messages_available, 0, 0);
}

void clean() {
    rbuf_destroy(buf);
    sem_destroy(&slots_available);
    sem_destroy(&messages_available);
}

void consumer(unsigned int id) {
    sem_wait(&messages_available);
    pthread_mutex_lock(&buf_mtx);
    rbuf_message_t message = rbuf_pop(buf);
    printf("- Consumer proc #%d extracted message: %d\n", id, message);
    pthread_mutex_unlock(&buf_mtx);
    sem_post(&slots_available);
}

void producer(unsigned int id, unsigned int message_id) {
    sem_wait(&slots_available);
    rbuf_message_t message = message_id;
    pthread_mutex_lock(&buf_mtx);
    rbuf_push(buf, message);
    printf("+ Producer proc #%d inserted message: %d\n", id, message);
    pthread_mutex_unlock(&buf_mtx);
    sem_post(&messages_available);
}
