#include <../include/memoria_kernel.h>
// #include <commons/collections/list.h>

void crear_proceso(int pid,
                   char *path_proceso)
{
    kernel_memoria_process *proceso = malloc(sizeof(kernel_memoria_process));
    if (proceso == NULL)
    {
        log_error(memoria_logger, "Error en la creacion del proceso.");
        abort();
    }
    proceso->pid = pid;
    proceso->path_proceso = path_proceso;
    proceso->tabla_paginas = list_create();

    list_add(listaProcesosMemoria, proceso);
    log_error(memoria_logger, "Creado proceso en memoria con id: %d y path: %s", pid, path_proceso);
    int confirmed = 1;
    send(fd_kernel, &confirmed, sizeof(int), 0);
}

void liberar_marcos(t_list *tabla_paginas)
{
    t_link_element *current = tabla_paginas->head;
    if (!current)
    {
        log_error(memoria_logger, "Error al liberar los marcos del proceso.");
        abort();
    }
    fila_tabla_paginas *contenido = current->data;
    while (current->next != NULL)
    {
        int marco = contenido->marco;
        marcos_libres[marco] = 0;
        current = current->next;
    }
}

void finalizar_proceso(int pid)
{
    int index = encontrar_proceso_memoria(pid);
    kernel_memoria_process *proceso = list_get(listaProcesosMemoria, index);
    liberar_marcos(proceso->tabla_paginas);
    list_remove(listaProcesosMemoria, index);
}

void atender_memoria_kernel(char marcos_libres[])
{
    bool control_key = 1;
    int pid;
    char *path;

    while (control_key)
    {
        int cod_op = recibir_operacion(fd_kernel);
        int size_paquete = recibir_int(fd_kernel);
        switch (cod_op)
        {
        case MENSAJE:
            // recibir_mensaje(cliente_fd);
            break;
        case PAQUETE:
            // lista = recibir_paquete(fd_kernel_dispatch);
            // log_info(cpu_logger, "Me llegaron los siguientes valores:\n");
            // list_iterate(lista, (void*) iterator);
            // break;
        case INICIAR_PROCESO_KERNEL: //[char* path][int tamanio][int prioridad]

            pid = recibir_int(fd_kernel);
            path = recibir_string(fd_kernel);

            sleep(RETARDO_RESPUESTA);
            crear_proceso(pid, path);
            break;
        case ELIMINAR_PROCESO_KERNEL_MEMORIA:
            sleep(RETARDO_RESPUESTA);
            pid = recibir_int(fd_kernel);
            finalizar_proceso(pid);
            break;
        case -1:
            log_error(memoria_logger, "El KERNEL se desconecto. Terminando servidor");
            control_key = 0;
            return EXIT_FAILURE;
            break;
        default:
            log_warning(memoria_logger, "Operacion desconocida de KERNEL");
            break;
        }
    }
}
