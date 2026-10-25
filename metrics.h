#ifndef METRICS_H
#define METRICS_H

#include "tcb.h"

// ESTRUCTURA PARA REGISTRAR CENTINELAS TERMINADOS Y METRICAS GLOBALES
typedef struct
{
    TCB **finished_tasks;      // Arreglo dinamico de punteros a TCB terminados
    int count;                 // Cantidad de tareas terminadas hasta el momento
    int capacity;              // Capacidad maxima del arreglo
    int total_simulation_time; // Ultimo tic global alcanzado en la simulacion
} MetricsCollector;

// CONTADOR CON LA CANTIDAD ESPERADA DE CENTINELAS
void metrics_init(MetricsCollector *collector, int total_expected_tasks);

// REGISTRAR TAREA TERMINADA
void metrics_registro_completadas(MetricsCollector *collector, TCB *task, int tg);

// CALCULO METRICAS
void metrics_calcular_reporte(MetricsCollector *collector);

// LIBERACION DE LA MEMORIA DE LOS TCB REGISTRADOS Y DEL COLECTOR
void metrics_destroy(MetricsCollector *collector);

#endif