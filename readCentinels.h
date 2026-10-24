#ifndef READCENTINELS_H
#define READCENTINELS_H

#include <stdbool.h>
#include "tcb.h"

// Estructura para almacenar las opciones de configuración
typedef struct {
    char *input_filepath;  // Ruta al archivo con el centinela
    bool verbose;          // Activar el modo depuración (-v)
} ConfigOptions;

void read_arguments(int argc, char *argv[], ConfigOptions *config);

TCB* read_input_file(const char *filepath, int *task_count);

#endif // READCENTINELS_H