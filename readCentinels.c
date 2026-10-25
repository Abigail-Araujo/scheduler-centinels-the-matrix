#include "readCentinels.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

#define MAX_LINE_LENGTH 256
#define OPT_INPUT 256  // Identificador unico para la opcion larga --input

void read_arguments(int argc, char *argv[], ConfigOptions *config) {
    config->input_filepath = NULL;
    config->verbose = false;

    static struct option long_options[] = {
        {"verbose", no_argument, NULL, 'v'}, // No requiere argumento
        {"input", required_argument, NULL, OPT_INPUT}, // Requiere argumento (ruta del archivo)
        {0, 0, 0, 0} // Marca el final de las opciones
    };

    int opt;
    while ((opt = getopt_long(argc, argv, "v", long_options, NULL)) != -1) {
        switch (opt) {
            case 'v':
                config->verbose = true;
                break;
            case OPT_INPUT:
                config->input_filepath = optarg;
                break;
            default:
                fprintf(stderr, "Uso: %s [-v] --input <input_file>\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    // VALIDAR --input SUMINISTRADO
    if (config->input_filepath == NULL) {
        fprintf(stderr, "Error: Falta el parametro obligatorio --input <input_file>\n");
        fprintf(stderr, "Uso: %s [-v] --input <input_file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
}

// INSERTAR NUEVO TCB EN LISTA ORDENADA ASCENDENTEMENTE
static TCB* insert_sorted_by_arrival(TCB *head, TCB *new_task) {
    if (head == NULL || new_task->arrivalTime < head->arrivalTime) {
        new_task->next = head;
        return new_task; 
    }

    TCB *current = head;
    while (current->next != NULL && current->next->arrivalTime <= new_task->arrivalTime) {
        current = current->next;
    }

    new_task->next = current->next;
    current->next = new_task;
    return head;
}

TCB*read_input_file(const char *filepath, int *task_count) {
    FILE *file = fopen(filepath, "r");
    if (!file) {
        perror("Error al abrir el archivo de entrada");
        exit(EXIT_FAILURE);
    }

    TCB *task_list_head = NULL;
    char line[MAX_LINE_LENGTH];
    int count = 0;
    int line_num = 0;

    while (fgets(line, sizeof(line), file)) {
        line_num++;

        // Saltar espacios en blanco iniciales
        char *ptr = line;
        while (*ptr == ' ' || *ptr == '\t') {
            ptr++;
        }

        // IGNORAR LINEAS VACIAS Y LO QUE INICIA CON "#"
        if (*ptr == '#' || *ptr == '\n' || *ptr == '\r' || *ptr == '\0') {
            continue;
        }

        // Parsear los 4 valores requeridos
        int arrival_time, id, raw_priority, burst;
        int parsed_items = sscanf(ptr, "%d %d %d %d", &arrival_time, &id, &raw_priority, &burst);

        if (parsed_items != 4) {
            fprintf(stderr, "Advertencia: Formato inválido en la línea %d. Ignorando.\n", line_num);
            continue;
        }

        // VALIDACION DEL RANGO DE PRIORIDAD (0, 1, 2)
        if (raw_priority < PRIORITY_HIGH || raw_priority > PRIORITY_LOW) {
            fprintf(stderr, "Error: Prioridad inválida (%d) en la línea %d. Debe ser 0, 1 o 2.\n", raw_priority, line_num);
            fclose(file);
            exit(EXIT_FAILURE);
        }

        // CREACION DEL TCB Y INSERCION ORDENADA A LA LISTA INICIAL
        PriorityLevel priority = (PriorityLevel)raw_priority;
        TCB *new_task = tcb_create(arrival_time, id, priority, burst);
        task_list_head = insert_sorted_by_arrival(task_list_head, new_task);
        count++;
    }

    fclose(file);
    *task_count = count;
    return task_list_head;
}