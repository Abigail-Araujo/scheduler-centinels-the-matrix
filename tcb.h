#ifndef TCB_H
#define TCB_H

#include <stdio.h>
#include <stdlib.h>

// ESTADOS
typedef enum {
    NEW,
    READY,
    RUNNING,
    INTERRUPTED,
    FINISHED
} TaskState;

// NIVELES DE PRIORIDAD
typedef enum {
    PRIORITY_HIGH = 0, // RR q=2
    PRIORITY_MEDIUM = 1, // RR q=4
    PRIORITY_LOW = 2 // FCFS
} PriorityLevel;

// TCB (estructura para representar un centinela)
typedef struct TCB {
    int id; 
    TaskState state; 
    PriorityLevel priority; // cola asignada (HIGH, MEDIUM o LOW)
    int arrivalTime; 
    int totalBurst; // rafaga total de CPU requerida
    int remainingBurst; // rafaga restante por ejecutar
    int usedQuantum; // quantum consumido acumulado en la ráfaga actual
    int waitTime; 
    int returnTime; // (completionTime - arrivalTime)

    int completionTime; // tic global exacto en el que finaliza
    int firstResponseTime; // tic global de primera respuesta (-1 si no ha iniciado)
    struct TCB *next; 
} TCB;

// INICIALIZAR TCB
static inline TCB* tcb_create(int arrivalTime, int id, PriorityLevel priority, int burst) {
    TCB *task = (TCB *)malloc(sizeof(TCB));
    if (!task) {
        perror("Error de asignación de memoria para TCB");
        exit(EXIT_FAILURE);
    }
    task->id = id;
    task->state = NEW;
    task->priority = priority;
    task->arrivalTime = arrivalTime;
    task->totalBurst = burst;
    task->remainingBurst = burst;
    task->usedQuantum = 0;
    task->waitTime = 0;
    task->returnTime = 0;
    task->completionTime = 0;
    task->firstResponseTime = -1;
    task->next = NULL;
    return task;
}

// LIBERAR MEMORIA DEL TCB
static inline void tcb_liberar(TCB *task) {
    if (task) {
        free(task);
    }
}

// RETORNAR TIPO DE CENTINELA COMO CADENA
static inline const char* tcb_tipoCentinela(PriorityLevel priority) {
    switch (priority) {
        case PRIORITY_HIGH: return "Command";
        case PRIORITY_MEDIUM: return "Driller";
        case PRIORITY_LOW: return "Heavy";
        default: return "Unknown";
    }
}

#endif