#include "queue.h"
#include <stdlib.h>

void queue_init(Queue *q) {
    if (q) {
        q->head = NULL;
        q->tail = NULL;
        q->size = 0;
    }
}

bool queue_is_empty(const Queue *q) {
    return (q == NULL || q->size == 0 || q->head == NULL);
}

int queue_size(const Queue *q) {
    if (queue_is_empty(q)) {
        return 0;
    }
    return q->size;
}

TCB* queue_getFirst(const Queue *q) {
    if (queue_is_empty(q)) {
        return NULL;
    }
    return q->head;
}

void queue_enqueue(Queue *q, TCB *task) {
    if (!q || !task) return;

    task->next = NULL;

    if (queue_is_empty(q)) {
        q->head = task;
        q->tail = task;
    } else {
        q->tail->next = task;
        q->tail = task;
    }
    q->size++;
}

TCB* queue_dequeue(Queue *q) {
    if (queue_is_empty(q)) {
        return NULL;
    }

    TCB *task = q->head;
    q->head = q->head->next;

    if (q->head == NULL) {
        q->tail = NULL;
    }

    task->next = NULL; // Desconectar el nodo extraído
    q->size--;
    return task;
}

void queue_colear(Queue *q, TCB *task) {
    if (!q || !task) return;

    if (queue_is_empty(q)) {
        task->next = NULL;
        q->head = task;
        q->tail = task;
    } else {
        task->next = q->head;
        q->head = task;
    }
    q->size++;
}