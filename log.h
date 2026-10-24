#ifndef LOG_H
#define LOG_H

#include <stdbool.h>
#include "tcb.h"
#include "queue.h"

// Mensaje inicial de las colas
void log_header(void);

// Eventos del Modo Estándar
void log_arrival(int tg, const TCB *task);
void log_quantum_expired(int tg, const TCB *task, int quantum);
void log_completed(int tg, const TCB *task);
// Expropiación y Despacho pero no se porque no salen en el ejemplo
// pero si en enunciado
void log_preempted(int tg, const TCB *running_task, PriorityLevel higher_prio);

// Eventos exclusivos del Modo Verbose (-v)
void log_verbose_tick_start(int tg);
void log_verbose_queues_state(const Queue queues[3], const TCB *running_task);
void log_verbose_dispatch(int tg, const TCB *task);

// Métricas Globales finales
void log_metrics(int total_time, int total_sentinels, double avg_waiting_time);

#endif