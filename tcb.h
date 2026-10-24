#ifndef TCB_H
#define TCB_H

#include <stdio.h>
#include <stdlib.h>

// Estados de los procesos centinelas
typedef enum {
    NEW,
    READY,
    RUNNING,
    INTERRUPTED,
    FINISHED
} TaskState;

// Niveles de prioridad de las colas:
typedef enum {
    PRIORITY_HIGH = 0, // Centinelas de Comando con Round Robin Q=2
    PRIORITY_MEDIUM = 1, // Centinelas Perforadores con Round Robin Q=4
    PRIORITY_LOW = 2 // Centinelas de Desgaste con FCFS
} PriorityLevel;

// Task Control Block - TCB (Estructura central para representar cada unidad de combate)
typedef struct TCB {
    int id; // Identificador único del centinela
    TaskState state; // Estado actual de ejecución
    PriorityLevel priority; // Cola asignada (HIGH, MEDIUM o LOW)
    int arrivalTime; // Tic global (Tg) en el que arriba al sistema
    int totalBurst; // Ráfaga total de CPU requerida
    int remainingBurst; // Ráfaga restante por ejecutar
    int usedQuantum; // Quantum consumido acumulado en la ráfaga actual
    int waitTime; // Tiempo acumulado esperando en cola de listos
    int returnTime; // Turnaround Time (completionTime - arrivalTime)

    // Campos auxiliares para soporte y métricas
    int completionTime; // Tic global exacto en el que finaliza
    int firstResponseTime; // Tic global de primera respuesta (-1 si no ha iniciado)
    struct TCB *next; // Puntero para la lista enlazada de la cola
} TCB;

// Función auxiliar para inicializar TCB
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

// Función para liberar la memoria de TCB
static inline void tcb_liberar(TCB *task) {
    if (task) {
        free(task);
    }
}

// Retornar el nombre táctico del centinela para coincidir con la salida esperada:
// [Type: Command], [Type: Driller] o [Type: Heavy]
static inline const char* tcb_tipoCentinela(PriorityLevel priority) {
    switch (priority) {
        case PRIORITY_HIGH: return "Command";
        case PRIORITY_MEDIUM: return "Driller";
        case PRIORITY_LOW: return "Heavy";
        default: return "Unknown";
    }
}

#endif