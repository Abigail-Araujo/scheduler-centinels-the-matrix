#include "colas.h"
#include "log.h"

Queue colas[3];
static TCB *siguiente_arribo;

void colas_inicializar(TCB *lista_pendiente)
{
    for (int i = 0; i < 3; i++)
    {
        queue_init(&colas[i]);
    }
    siguiente_arribo = lista_pendiente;
}

void admitir_centinela(int tic)
{
    while (siguiente_arribo && siguiente_arribo->arrivalTime <= tic)
    {
        TCB *task = siguiente_arribo;
        siguiente_arribo = siguiente_arribo->next;
        task->next = NULL;
        task->state = READY;
        queue_enqueue(&colas[task->priority], task);
        log_arrival(tic, task);
    }
}

void reencolar_final(TCB *task)
{
    if (task && task->priority >= PRIORITY_HIGH && task->priority <= PRIORITY_LOW)
    {
        queue_enqueue(&colas[task->priority], task);
    }
}

void reencolar_inicio(TCB *task)
{
    if (task && task->priority >= PRIORITY_HIGH && task->priority <= PRIORITY_LOW)
    {
        queue_colear(&colas[task->priority], task);
    }
}

TCB *extraer_colaMayorPrioridad(void)
{
    for (int i = PRIORITY_HIGH; i <= PRIORITY_LOW; i++)
    {
        if (!queue_is_empty(&colas[i]))
        {
            return queue_dequeue(&colas[i]);
        }
    }
    return NULL;
}

int obtener_PrioridadMayorDisponible(void)
{
    for (int i = PRIORITY_HIGH; i <= PRIORITY_LOW; i++)
    {
        if (!queue_is_empty(&colas[i]))
        {
            return i;
        }
    }
    return -1;
}