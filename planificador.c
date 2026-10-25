#include "planificador.h"
#include "colas.h"
#include "log.h"

#include <stdio.h>
#include <stdbool.h>
#include <errno.h>

// VARIABLES GLOBALES
int tic_global = 0;
pthread_mutex_t mutex_colas;
TCB *proceso_activo = NULL;
static pthread_cond_t condicion_tic;
static int tic_cpu_completado = 0;
static int tareas_completadas = 0;
static int tareas_totales = 0;
static TCB *tarea_completada_pendiente = NULL;
static MetricsCollector *metricas_simulacion = NULL;
static bool modo_verbose = false;
static bool simulacion_activa = false;

// FUNCION AUX
void cambiar_estado(TCB *task, TaskState nuevo_estado)
{
    if (task != NULL)
    {
        task->state = nuevo_estado;
    }
}

// HILO MAESTRO
void *rutina_reloj(void *arg)
{
    (void)arg; // evitar advertencia de variable no utilizada

    while (true)
    {
        pthread_mutex_lock(&mutex_colas);

        if (!simulacion_activa)
        {
            pthread_mutex_unlock(&mutex_colas);
            break;
        }

        tic_global++;
        admitir_centinela(tic_global);

        pthread_cond_broadcast(&condicion_tic);
        while (simulacion_activa && tic_cpu_completado < tic_global)
        {
            pthread_cond_wait(&condicion_tic, &mutex_colas);
        }

        pthread_mutex_unlock(&mutex_colas);
    }

    return NULL;
}

// HILO TRABAJADOR
void *rutina_cpu(void *arg)
{
    (void)arg;

    pthread_mutex_lock(&mutex_colas);

    while (simulacion_activa)
    {
        while (simulacion_activa && tic_cpu_completado >= tic_global)
        {
            pthread_cond_wait(&condicion_tic, &mutex_colas);
        }

        if (!simulacion_activa)
        {
            break;
        }

        if (modo_verbose)
        {
            log_v_separador_tick(tic_global);
        }

        if (tarea_completada_pendiente != NULL)
        {
            metrics_registro_completadas(metricas_simulacion,
                                         tarea_completada_pendiente, tic_global);
            tarea_completada_pendiente = NULL;
            tareas_completadas++;

            if (tareas_completadas >= tareas_totales)
            {
                simulacion_activa = false;
            }
        }

        if (!simulacion_activa)
        {
            tic_cpu_completado = tic_global;
            pthread_cond_broadcast(&condicion_tic);
            break;
        }

        if (proceso_activo != NULL && proceso_activo->priority != PRIORITY_LOW)
        {
            int quantum = (proceso_activo->priority == PRIORITY_HIGH) ? 2 : 4;

            if (proceso_activo->usedQuantum >= quantum)
            {
                int prioridad_lista = obtener_PrioridadMayorDisponible();
                if (prioridad_lista >= PRIORITY_HIGH &&
                    prioridad_lista <= (int)proceso_activo->priority)
                {
                    log_quantum_expired(tic_global, proceso_activo, quantum);
                }
                cambiar_estado(proceso_activo, INTERRUPTED);
                proceso_activo->usedQuantum = 0;
                cambiar_estado(proceso_activo, READY);
                reencolar_final(proceso_activo);
                proceso_activo = NULL;
            }
        }

        // EXPROPIACION POR PRIORIDAD
        if (proceso_activo != NULL)
        {
            int prioridad_mayor = obtener_PrioridadMayorDisponible();

            if (prioridad_mayor >= PRIORITY_HIGH &&
                prioridad_mayor < (int)proceso_activo->priority)
            {
                log_preempted(tic_global, proceso_activo,
                              (PriorityLevel)prioridad_mayor);
                cambiar_estado(proceso_activo, INTERRUPTED);
                cambiar_estado(proceso_activo, READY);
                reencolar_inicio(proceso_activo);
                proceso_activo = NULL;
            }
        }

        // DESPACHO Y EJECUCION EN EL MISMO TIC
        if (proceso_activo == NULL)
        {
            proceso_activo = extraer_colaMayorPrioridad();

            if (proceso_activo != NULL)
            {
                cambiar_estado(proceso_activo, RUNNING);
                if (proceso_activo->firstResponseTime == -1)
                    proceso_activo->firstResponseTime = tic_global;
                if (modo_verbose)
                    log_v_dispatch(tic_global, proceso_activo);
            }
        }

        if (proceso_activo != NULL)
        {
            proceso_activo->remainingBurst--;
            if (proceso_activo->priority != PRIORITY_LOW)
            {
                proceso_activo->usedQuantum++;
            }

            if (proceso_activo->remainingBurst == 0)
            {
                tarea_completada_pendiente = proceso_activo;
                proceso_activo = NULL;
            }
        }

        if (modo_verbose)
        {
            log_v_estado_colas(colas, proceso_activo);
        }

        tic_cpu_completado = tic_global;
        pthread_cond_broadcast(&condicion_tic);

        if (!simulacion_activa)
        {
            pthread_cond_broadcast(&condicion_tic);
        }
    }

    pthread_mutex_unlock(&mutex_colas);
    return NULL;
}

int iniciar_planificador(TCB *lista_pendiente, int total_tareas,
                         MetricsCollector *metricas, bool verbose)
{
    pthread_t reloj, cpu;
    int resultado;

    if (total_tareas <= 0)
    {
        return 0;
    }

    tareas_totales = total_tareas;
    tareas_completadas = 0;
    tarea_completada_pendiente = NULL;
    metricas_simulacion = metricas;
    modo_verbose = verbose;
    tic_global = -1;
    tic_cpu_completado = -1;
    colas_inicializar(lista_pendiente);

    if (pthread_mutex_init(&mutex_colas, NULL) != 0)
    {
        perror("Error de inicialización del mutex de colas");
        return -1;
    }

    resultado = pthread_cond_init(&condicion_tic, NULL);
    if (resultado != 0)
    {
        errno = resultado;
        perror("Error de inicialización de la condición del tic");
        pthread_mutex_destroy(&mutex_colas);
        return -1;
    }

    pthread_mutex_lock(&mutex_colas);
    simulacion_activa = true;
    pthread_mutex_unlock(&mutex_colas);

    resultado = pthread_create(&reloj, NULL, rutina_reloj, NULL);
    if (resultado != 0)
    {
        errno = resultado;
        perror("Error al crear el hilo reloj");
        pthread_mutex_lock(&mutex_colas);
        simulacion_activa = false;
        pthread_mutex_unlock(&mutex_colas);
        pthread_cond_destroy(&condicion_tic);
        pthread_mutex_destroy(&mutex_colas);
        return -1;
    }

    resultado = pthread_create(&cpu, NULL, rutina_cpu, NULL);
    if (resultado != 0)
    {
        errno = resultado;
        perror("Error al crear el hilo CPU");
        pthread_mutex_lock(&mutex_colas);
        simulacion_activa = false;
        pthread_cond_broadcast(&condicion_tic);
        pthread_mutex_unlock(&mutex_colas);
        pthread_join(reloj, NULL);
        pthread_cond_destroy(&condicion_tic);
        pthread_mutex_destroy(&mutex_colas);
        return -1;
    }

    pthread_join(reloj, NULL);
    pthread_join(cpu, NULL);
    pthread_cond_destroy(&condicion_tic);
    pthread_mutex_destroy(&mutex_colas);

    return 0;
}

/*
NOTA: FUENTES
Para recordar lo del mutex:

https://www.youtube.com/watch?v=oq29KUy29iQ&t=187s

https://www.youtube.com/watch?v=sD__p9cbQpc
*/