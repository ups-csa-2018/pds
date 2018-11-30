#include <assert.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
typedef enum { alpha, beta, gamma, max_message_type_t } message_type_t;
typedef struct { message_type_t type; unsigned int message; } rbuf_message_t;
#include "rbuf.h"

rbuf_t *buf;
sem_t slots_available, messages_available;
pthread_mutex_t type_mtx[max_message_type_t], buf_mtx;

void init(unsigned int buf_size, unsigned int consumers, unsigned int producers) {
    assert(producers % max_message_type_t == 0);
    assert(consumers >= max_message_type_t);
    buf = rbuf_create(buf_size);
    sem_init(&slots_available, 0, buf_size);
    sem_init(&messages_available, 0, 0);
    for (int i = 0; i < max_message_type_t; i++)
        pthread_mutex_lock(&type_mtx[i]);
}

void clean() {
    rbuf_destroy(buf);
    sem_destroy(&slots_available);
    sem_destroy(&messages_available);
}

message_type_t get_producer_type(unsigned int producer_id) {
    return (message_type_t)(producer_id % max_message_type_t);
}

void consumer(unsigned int id) {
    message_type_t type = get_producer_type(id);
    pthread_mutex_lock(&type_mtx[type]);
    pthread_mutex_lock(&buf_mtx);
    rbuf_message_t message = rbuf_pop(buf);
    printf("- Consumer proc #%d extracted message: %u,%d\n",
        id, message.message, message.type);
    if (!rbuf_is_empty(buf))
        pthread_mutex_unlock(&type_mtx[rbuf_front(buf).type]);
    pthread_mutex_unlock(&buf_mtx);
    sem_post(&slots_available);
}

void producer(unsigned int id, unsigned int message_id) {
    message_type_t type = get_producer_type(id);
    sem_wait(&slots_available);
    rbuf_message_t message = { type, message_id };
    pthread_mutex_lock(&buf_mtx);
    rbuf_push(buf, message);
    printf("+ Producer proc #%d inserted message: %u,%d\n",
        id, message.message, message.type);
    if (rbuf_count(buf) == 1)
        pthread_mutex_unlock(&type_mtx[rbuf_front(buf).type]);
    pthread_mutex_unlock(&buf_mtx);
}
