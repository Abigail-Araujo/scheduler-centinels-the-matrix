#ifndef PLANIFICADOR_H
#define PLANIFICADOR_H

#include <pthread.h>
#include <stdbool.h>
#include "tcb.h"
#include "metrics.h"

extern int tic_global;
extern pthread_mutex_t mutex_colas; // para proteger el acceso concurrente a las colas y variables compartidas
extern TCB *proceso_activo;         // puntero al proceso actualmente en ejecución

int iniciar_planificador(TCB *lista_pendiente, int total_tareas,
                         MetricsCollector *metricas, bool verbose);

#endif