#pragma once

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>

typedef struct {
    unsigned int pos_read;
    unsigned int pos_write;
    unsigned int count;
    unsigned int size;
    rbuf_message_t *data;
} rbuf_t;

rbuf_t *rbuf_create(const unsigned int size)
{
    rbuf_t *buf = malloc(sizeof(rbuf_t));
    buf->data = malloc(sizeof(rbuf_message_t) * size);
    buf->pos_read = 0;
    buf->pos_write = 0;
    buf->size = size;
    buf->count = 0;

    return buf;
}

void rbuf_destroy(rbuf_t *buf)
{
    free(buf->data);
    free(buf);
}

bool rbuf_is_full(const rbuf_t *buf)
{
    return buf->size == buf->count;
}

bool rbuf_is_empty(const rbuf_t *buf)
{
    return 0 == buf->count;
}

unsigned int rbuf_count(const rbuf_t *buf)
{
    return buf->count;
}

void rbuf_push(rbuf_t *buf, const rbuf_message_t message)
{
    assert(!rbuf_is_full(buf));

    buf->data[buf->pos_write] = message;
    buf->pos_write = (buf->pos_write + 1) % buf->size;
    buf->count++;
}

rbuf_message_t rbuf_pop(rbuf_t *buf)
{
    assert(!rbuf_is_empty(buf));

    rbuf_message_t message = buf->data[buf->pos_read];
    buf->pos_read = (buf->pos_read + 1) % buf->size;
    buf->count--;

    return message;
}

rbuf_message_t rbuf_front(const rbuf_t *buf)
{
    assert(!rbuf_is_empty(buf));

    return buf->data[buf->pos_read];
}
