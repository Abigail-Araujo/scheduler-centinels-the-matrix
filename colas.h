#ifndef COLAS_H
#define COLAS_H

#include "queue.h"

extern Queue colas[3];

void colas_inicializar(TCB *lista_pendiente);
void admitir_centinela(int tic);
void reencolar_final(TCB *task);
void reencolar_inicio(TCB *task);
TCB *extraer_colaMayorPrioridad(void);
int obtener_PrioridadMayorDisponible(void);

#endif