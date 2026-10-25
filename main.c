#include "log.h"
#include "metrics.h"
#include "planificador.h"
#include "readCentinels.h"

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    ConfigOptions config;
    MetricsCollector metricas;
    TCB *lista_pendiente;
    int total_tareas = 0;

    read_arguments(argc, argv, &config);
    lista_pendiente = read_input_file(config.input_filepath, &total_tareas);

    metrics_init(&metricas, total_tareas);
    log_header();

    if (iniciar_planificador(lista_pendiente, total_tareas,
                             &metricas, config.verbose) != 0)
    {
        fprintf(stderr, "Error: no se pudo iniciar el planificador.\n");
        metrics_destroy(&metricas);
        return EXIT_FAILURE;
    }

    metrics_calculate_report(&metricas);
    metrics_destroy(&metricas);
    return EXIT_SUCCESS;
}
