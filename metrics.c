#include "metrics.h"
#include "log.h"
#include <stdio.h>
#include <stdlib.h>

void metrics_init(MetricsCollector *collector, int total_expected_tasks)
{
    if (!collector)
        return;
    collector->capacity = (total_expected_tasks > 0) ? total_expected_tasks : 10;
    collector->count = 0;
    collector->total_simulation_time = 0;
    collector->finished_tasks = (TCB **)malloc(sizeof(TCB *) * collector->capacity);
    if (!collector->finished_tasks)
    {
        perror("Error al asignar memoria en el colector de metricas");
        exit(EXIT_FAILURE);
    }
}

void metrics_registro_completadas(MetricsCollector *collector, TCB *task, int tg)
{
    if (!collector || !task)
        return;

    // GUARDADO DE MARCAS DE AUDITORIA EN LA TCB
    task->state = FINISHED;
    task->completionTime = tg;
    task->returnTime = tg - task->arrivalTime;
    task->waitTime = task->returnTime - task->totalBurst;

    log_completed(tg, task);

    // ALMACENAMIENTO EN EL ARREGLO PARA EL REPORTE FINAL
    if (collector->count < collector->capacity)
    {
        collector->finished_tasks[collector->count++] = task;
    }
    collector->total_simulation_time = tg;
}

void metrics_calcular_reporte(MetricsCollector *collector)
{
    if (!collector)
        return;

    double total_wait_time = 0.0;
    for (int i = 0; i < collector->count; i++)
    {
        total_wait_time += collector->finished_tasks[i]->waitTime;
    }

    double avg_wait_time = (collector->count > 0) ? (total_wait_time / collector->count) : 0.0;

    log_metrics(collector->total_simulation_time, collector->count, avg_wait_time);
}

void metrics_destroy(MetricsCollector *collector)
{
    if (!collector)
        return;
    if (collector->finished_tasks)
    {
        for (int i = 0; i < collector->count; i++)
        {
            tcb_liberar(collector->finished_tasks[i]);
        }
        free(collector->finished_tasks);
        collector->finished_tasks = NULL;
    }
}