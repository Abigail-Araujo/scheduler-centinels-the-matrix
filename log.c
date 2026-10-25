#include "log.h"
#include <stdio.h>

void log_header(void) {
    printf("Fixed parameters: Queue 0 [Q=2] | Queue 1 [Q=4] | Queue 2 [FCFS]\n\n");
}

void log_arrival(int tg, const TCB *task) {
    if (!task) return;
    printf("[T-%04d] ARRIVAL: Sentinel ID:%d [Type: %s]\n",
           tg, task->id, tcb_tipoCentinela(task->priority));
}

void log_quantum_expired(int tg, const TCB *task, int quantum) {
    if (!task) return;
    printf("[T-%04d] QUANTUM_EXPIRED: Sentinel %d yields CPU via RR rotation (Q=%d)\n",
           tg, task->id, quantum);
}

void log_completed(int tg, const TCB *task) {
    if (!task) return;
    printf("[T-%04d] COMPLETED: Sentinel ID:%d -> Turnaround Time: %d ticks\n",
           tg, task->id, task->returnTime);
}

void log_preempted(int tg, const TCB *running_task, PriorityLevel higher_prio) {
    if (!running_task) return;
    printf("[T-%04d] PREEMPTION: Sentinel %d [Type: %s] preempted by higher priority queue (%s)\n",
           tg, running_task->id, tcb_tipoCentinela(running_task->priority),
           tcb_tipoCentinela(higher_prio));
}

void log_verbose_tick_start(int tg) {
    printf("\n--- [TIC %04d] ---\n", tg);
}

void log_verbose_queues_state(const Queue queues[3], const TCB *running_task) {
    printf("  [CPU]: ");
    if (running_task) {
        printf("Sentinel %d (RemBurst: %d, UsedQ: %d)\n",
               running_task->id, running_task->remainingBurst, running_task->usedQuantum);
    } else {
        printf("IDLE\n");
    }

    for (int i = 0; i < 3; i++) {
        printf("  [Queue %d (%s) - Size %d]: ", i, tcb_tipoCentinela((PriorityLevel)i), queue_size(&queues[i]));
        TCB *curr = queues[i].head;
        if (!curr) {
            printf("EMPTY");
        }
        while (curr) {
            printf("[ID:%d Rem:%d] ", curr->id, curr->remainingBurst);
            curr = curr->next;
        }
        printf("\n");
    }
}

void log_verbose_dispatch(int tg, const TCB *task) {
    if (task) {
        printf("[T-%04d] DISPATCH: Sentinel %d assigned to CPU_0\n", tg, task->id);
    }
}

void log_metrics(int total_time, int total_sentinels, double avg_waiting_time) {
    double throughput = (total_time > 0) ? ((double)total_sentinels / (double)total_time) : 0.0;

    printf("\n==================== GLOBAL PERFORMANCE METRICS ====================\n");
    printf("\t- Total Simulation Time  : %d clock ticks.\n", total_time);
    printf("\t- System Throughput      : %.2f sentinels/tick.\n", throughput);
    printf("\t- Average Waiting Time   : %.2f ticks.\n", avg_waiting_time);
    printf("====================================================================\n");
}