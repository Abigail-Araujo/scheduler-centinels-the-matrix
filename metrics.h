#ifndef METRICS_H
#define METRICS_H

#include "tcb.h"

// Estructura para registrar los centinelas terminados y calcular métricas globales
typedef struct {
    TCB **finished_tasks;      // Arreglo dinámico de punteros a TCB terminados
    int count;                 // Cantidad de tareas terminadas hasta el momento
    int capacity;              // Capacidad máxima del arreglo
    int total_simulation_time; // Último tic global alcanzado en la simulación
} MetricsCollector;

// Inicializa el colector de métricas con la cantidad total esperada de centinelas.
void metrics_init(MetricsCollector *collector, int total_expected_tasks);

// Registra una tarea terminada, calculando de inmediato su returnTime y waitTime,
// e invoca a log_completed para mostrar la traza requerida.
void metrics_record_completion(MetricsCollector *collector, TCB *task, int tg);

// Calcula las métricas finales y llama a log_metrics(...) para imprimirlas.
void metrics_calculate_report(MetricsCollector *collector);

// Libera toda la memoria de los TCB registrados y del colector.
void metrics_destroy(MetricsCollector *collector);

#endif