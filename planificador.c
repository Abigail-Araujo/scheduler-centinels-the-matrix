#include "planificador.h"

#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>

// VARIABLES GLOBALES
int tic_global = 0;
pthread_mutex_t mutex_colas;
TCB *proceso_activo = NULL;

// PARA ABI: me tienes que proveer de esto
extern bool simulacion_activa; 

extern void admitir_centinela(int tic);
extern void reencolar_final(TCB *task);
extern void reencolar_inicio(TCB *task);
extern TCB* extraer_colaMayorPrioridad(void);
extern int obtener_PrioridadMayorDisponible(void);

// FUNCION AUX
void cambiar_estado(TCB *task, TaskState nuevo_estado) {
    if (task != NULL) {
        task->state = nuevo_estado;
    }
}

// HILO MAESTRO
void* rutina_reloj (void *arg)
{
    (void)arg; // evitar advertencia de variable no utilizada

    while (simulacion_activa)
    {
        pthread_mutex_lock(&mutex_colas); 

        tic_global++;
        admitir_centinela(tic_global);

        pthread_mutex_unlock(&mutex_colas);
        usleep(1000); // 1 ms
    }

    return NULL;
}

// HILO TRABAJADOR
void* rutina_cpu (void *arg)
{
    (void)arg;

    while (simulacion_activa)
    {
        pthread_mutex_lock(&mutex_colas);

        if (proceso_activo != NULL)
        {
            // EXPIRACION DEL QUANTUM
            if (proceso_activo->priority != PRIORITY_LOW && proceso_activo->remainingBurst > 0)
            {
                int quantum = (proceso_activo->priority == PRIORITY_HIGH) ? 2 : 4;

                if (proceso_activo->usedQuantum >= quantum)
                {
                    cambiar_estado(proceso_activo, INTERRUPTED);
                    proceso_activo->usedQuantum = 0;

                    cambiar_estado(proceso_activo, READY);
                    reencolar_final(proceso_activo);
                    proceso_activo = NULL;
                }
            }

            // EXPROPIACION POR PRIORIDAD
            int prioridad_mayor = obtener_PrioridadMayorDisponible();

            if (prioridad_mayor < proceso_activo->priority)
            {
                cambiar_estado(proceso_activo, INTERRUPTED);
                // conserva el quantum consumido, no se reinicia en este caso

                cambiar_estado(proceso_activo, READY);
                reencolar_inicio(proceso_activo);
                proceso_activo = NULL;
            }

            // EJECUCION DEL PROCESO ACTIVO
            proceso_activo->remainingBurst--;
            if (proceso_activo->priority != PRIORITY_LOW) proceso_activo->usedQuantum++;
        

            if (proceso_activo->remainingBurst == 0)
            {
                cambiar_estado(proceso_activo, FINISHED);
                proceso_activo->completionTime = tic_global;
                proceso_activo->returnTime = tic_global - proceso_activo->arrivalTime;
                proceso_activo->waitTime = proceso_activo->returnTime - proceso_activo->totalBurst;

                proceso_activo = NULL;
            }
        }
        else
        {
            // DESPACHO
            proceso_activo = extraer_colaMayorPrioridad();

            if (proceso_activo != NULL)
            {
                cambiar_estado(proceso_activo, RUNNING);
                if (proceso_activo->firstResponseTime == -1) proceso_activo->firstResponseTime = tic_global;
            }
        }

        pthread_mutex_unlock(&mutex_colas);
        usleep(1000); 
    }
    return NULL;
}

int iniciar_planificador(void)
{
    pthread_t reloj, cpu;

    // INICIALIZAR MUTEX
    if (pthread_mutex_init(&mutex_colas, NULL) != 0) {
        perror("Error de inicialización del mutex de colas");
        return -1;
    }

    // CREAR HILOS
    pthread_create(&reloj, NULL, rutina_reloj, NULL);
    pthread_create(&cpu, NULL, rutina_cpu, NULL);

    // ESPERAR A QUE TERMINEN LOS HILOS
    pthread_join(reloj, NULL);
    pthread_join(cpu, NULL);

    // DESTRUIR MUTEX
    pthread_mutex_destroy(&mutex_colas);

    return 0;
}

/*
NOTA: FUENTES
Para recordar lo del mutex:

https://www.youtube.com/watch?v=oq29KUy29iQ&t=187s

https://www.youtube.com/watch?v=sD__p9cbQpc
*/