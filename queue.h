#ifndef QUEUE_H
#define QUEUE_H

#include <stdbool.h>
#include "tcb.h"

typedef struct
{
    TCB *head;
    TCB *tail;
    int size;
} Queue;

void queue_init(Queue *q);

void queue_enqueue(Queue *q, TCB *task);

TCB *queue_dequeue(Queue *q);

void queue_colear(Queue *q, TCB *task);

TCB *queue_getFirst(const Queue *q);

bool queue_is_empty(const Queue *q);

int queue_size(const Queue *q);

#endif