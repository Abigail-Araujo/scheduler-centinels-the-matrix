#ifndef LOG_H
#define LOG_H

#include <stdbool.h>
#include "tcb.h"
#include "queue.h"

// MENSAJE INICIAL DE LAS COLAS
void log_inicio(void);

// EVENTOS MODO ESTANDAR
void log_arrival(int tg, const TCB *task);
void log_quantum_expired(int tg, const TCB *task, int quantum);
void log_completed(int tg, const TCB *task);
// Expropiacion y Despacho pero no se porque no salen en el ejemplo
// pero si en enunciado
void log_preempted(int tg, const TCB *running_task, PriorityLevel higher_prio);

// EVENTOS MODO VERBOSE (-v)
void log_v_separador_tick(int tg);
void log_v_estado_colas(const Queue queues[3], const TCB *running_task);
void log_v_dispatch(int tg, const TCB *task);

// METRICAS GLOBALES FINALES
void log_metrics(int total_time, int total_sentinels, double avg_waiting_time);

#endif