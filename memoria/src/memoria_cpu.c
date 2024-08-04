#include <../include/memoria_cpu.h>
#include <math.h>

void atender_peticion_instruccion()
{
    int pidBusqueda = recibir_int(fd_cpu);
    int program_counter = 0;
    uint32_t var = 0;
    recv(fd_cpu, &var, sizeof(uint32_t), MSG_WAITALL);
    program_counter = (int)var;
    log_info(memoria_logger, "pid: %d, program_counter: %d", pidBusqueda, program_counter);

    kernel_memoria_process *data = NULL; // malloc(sizeof(kernel_memoria_process));
    data = (kernel_memoria_process *)list_get(listaProcesosMemoria, pidBusqueda);
    char *instruccion = obtenerInstruccion(data->path_proceso, program_counter);

    t_paquete *paquete_enviar = crear_paquete(SOLICITUD_INSTRUCCION);
    // agregar_a_paquete(paquete_enviar, instruccion, strlen(instruccion) + 1);
    agregar_string_a_paquete(paquete_enviar, instruccion);
    usleep((RETARDO_RESPUESTA) * 1000); // Duerme el proceso por RETARDO y luego continua
    enviar_paquete(paquete_enviar, fd_cpu);

    eliminar_paquete(paquete_enviar);
    log_info(memoria_logger, "Instruccion enviada a CPU(%s), PID(%i), PC(%i)", instruccion, pidBusqueda, program_counter);
}

char *obtenerInstruccion(char *pathInstrucciones, int program_counter)
{
    FILE *file;
    file = fopen(pathInstrucciones, "rt");
    if (file == NULL)
    {
        perror("Error al abrir el archivo");
        return NULL;
    }

    int numLineaActual = 0;
    char lineaActual[1000];
    char *instruccion = NULL;

    while (fgets(lineaActual, sizeof(lineaActual), file) != NULL)
    {
        if (numLineaActual == program_counter)
        {
            // Eliminamos el salto de línea si existe
            char *pos;
            if ((pos = strchr(lineaActual, '\n')) != NULL)
                *pos = '\0';

            // Asignamos memoria para la instrucción y copiamos la línea actual
            instruccion = malloc(strlen(lineaActual) + 1);
            strcpy(instruccion, lineaActual);

            break; // Terminamos el bucle una vez encontramos la línea deseada
        }
        numLineaActual++;
    }

    fclose(file);
    return instruccion;
}

bool es_marco_valido(t_list *tabla_paginas, int marco)
{
    t_link_element *current = tabla_paginas->head;
    if (!current)
    {
        log_error(memoria_logger, "Error. No hay elementos en la tabla de paginas.");
        abort();
    }
    fila_tabla_paginas *contenido = current->data;
    while (current->next != NULL && contenido->marco != marco)
    {
        current = current->next;
    }
    if (contenido->marco == marco)
        return true;
    else
        return false;
}

int obtener_sig_marco(t_list *tabla_paginas, int marco)
{
    t_link_element *current = tabla_paginas->head;
    if (!current)
    {
        log_error(memoria_logger, "Error. No hay elementos en la tabla de paginas.");
        abort();
    }
    fila_tabla_paginas *contenido = current->data;
    while (current->next != NULL && contenido->marco != marco)
    {
        current = current->next;
    }
    if (contenido->marco == marco)
    {
        fila_tabla_paginas *sig = current->next->data;
        return sig->marco;
    }
    else
        return -1;
}

void liberar_marco(t_list *tabla_paginas, int pagina)
{
    t_link_element *current = tabla_paginas->head;

    if (!current)
    {
        log_error(memoria_logger, "El proceso no posee elementos en su tabla de paginas.");
        abort();
    }

    fila_tabla_paginas *contenido = current->data;
    int index = 0;
    while (current->next != NULL && contenido->pagina != pagina)
    {
        current = current->next;
        index++;
    }

    if (contenido->pagina != pagina)
    {
        log_error(memoria_logger, "No se encontro la pagina solicitada.");
        abort();
    }
    marcos_libres[contenido->marco] = 0;
    list_remove(tabla_paginas, index);
}

int hay_marco_libre()
{
    int num_frames = sizeof(char) * TAM_MEMORIA / TAM_PAGINA;
    bool found = false;
    int i = 0;
    while (i < (TAM_MEMORIA / TAM_PAGINA) && found == false)
    {
        if (marcos_libres[i] == 0)
            found = true;
        i++;
    }

    if (found)
        return i;
    else
        return -1;
}

int asignar_marco(t_list *tabla_paginas)
{
    int marco_libre = hay_marco_libre();
    if (marco_libre == -1)
    {
        return -1;
    }
    else
    {
        fila_tabla_paginas *nueva_fila;
        nueva_fila->marco = marco_libre;
        nueva_fila->pagina = tabla_paginas->elements_count;
        list_add(tabla_paginas, nueva_fila);
        marcos_libres[marco_libre] = 1;
    }
}

int resize(int nuevo_tamanio, int pid)
{
    kernel_memoria_process *proceso = encontrar_proceso_memoria(pid);
    int paginas_proceso = proceso->tabla_paginas->elements_count;
    int tamanio_actual = paginas_proceso * TAM_PAGINA;
    if (tamanio_actual == nuevo_tamanio)
    {
        return 0;
    }
    else if (tamanio_actual > nuevo_tamanio)
    {
        while (tamanio_actual > nuevo_tamanio)
        {
            liberar_marco(proceso->tabla_paginas, paginas_proceso);
            tamanio_actual -= TAM_PAGINA;
            paginas_proceso--;
        }
        return 1;
    }
    else if (tamanio_actual < nuevo_tamanio)
    {
        while (tamanio_actual < nuevo_tamanio)
        {
            int marco_asignado = asignar_marco(proceso->tabla_paginas);
            if (marco_asignado == -1)
            {
                return -1;
            }
            tamanio_actual += TAM_PAGINA;
        }
        return 1;
    }
    return 0;
}

// Dado un codigo de operacion, realiza lectura o escritura sobre la memoria.
void *operar_memoria(int pid, int marco, int desplazamiento, void *buffer, size_t cant_bytes, char op)
{
    log_error(memoria_logger, "Buscando dato");
    if ((marco * TAM_PAGINA + desplazamiento + cant_bytes) > TAM_MEMORIA)
    {
        log_error(memoria_logger, "Lectura invalida. La lectura solicitada supera el espacio de la memoria.");
        abort();
    }

    kernel_memoria_process *proceso = encontrar_proceso_memoria(pid);
    log_error(memoria_logger, "Sali de encontrar proceso");
    log_error(memoria_logger, "PID: %d, path obtenido: %s", proceso->pid, proceso->path_proceso);
    if (proceso->path_proceso == NULL)
    {
        log_error(memoria_logger, "Error. No se pudo encontrar el proceso en la memoria.");
        abort();
    }
    log_error(memoria_logger, "proceso encontrado en teoria.. a ver? %d", list_size(proceso->tabla_paginas));
    t_list *tabla_paginas = proceso->tabla_paginas;

    if (!es_marco_valido(tabla_paginas, marco))
    {
        log_error(memoria_logger, "El marco no se encuentra en la tabla de paginas del proceso que solicita la lectura.");
        abort();
    }

    if (op != 'r' && op != 'w')
    {
        log_error(memoria_logger, "Inserte una operacion de memoria valida.");
        abort();
    }

    int bytes_leidos = 0;
    int marco_actual = marco;
    void *origen;
    void *destino;

    while (bytes_leidos < cant_bytes)
    {
        if (marco_actual == -1)
        {
            log_error(memoria_logger, "Error en la lectura de los datos. Se intento leer mas bytes que los que el proceso tiene asignado");
            abort();
        }

        if (op == 'r')
        {
            origen = memoria_usuario + marco_actual * TAM_PAGINA;
            destino = buffer + bytes_leidos;
        }
        if (op == 'w')
        {
            destino = memoria_usuario + marco_actual * TAM_PAGINA;
            origen = buffer + bytes_leidos;
        }

        // Si es la primer pagina, le sumo el desplazamiento.
        if (bytes_leidos == 0)
        {
            memcpy(destino, origen, TAM_PAGINA - desplazamiento);
            marco_actual = obtener_sig_marco(tabla_paginas, marco);
            bytes_leidos += desplazamiento;
        }
        // Si es la ultima pagina, leo los bytes restantes.
        else if (cant_bytes - bytes_leidos < TAM_PAGINA)
        {
            int bytes_restantes = cant_bytes - bytes_leidos;
            memcpy(destino + bytes_leidos, origen, bytes_restantes);
        }
        // Si no es ni la primer ni la ultima pagina, leo la pagina completa y cargo el siguiente marco.
        else
        {
            memcpy(destino, origen, TAM_PAGINA);
            marco_actual = obtener_sig_marco(tabla_paginas, marco);
            bytes_leidos += TAM_PAGINA;
        }
    }
    // Obs: El valor de retorno solo tiene sentido para la operacion de lectura
    return destino;
}

bool coincide_pagina(fila_tabla_paginas *fila)
{
    if (fila->pagina == pagina_buscada)
    {
        return true;
    }
    return false;
}

int consultar_tabla_paginas(int pid, int pagina)
{
    pagina_buscada = pagina;

    int index = encontrar_proceso_memoria(pid);
    kernel_memoria_process *proceso = list_get(listaProcesosMemoria, index);
    t_list *tabla_paginas = proceso->tabla_paginas;
    int marco = list_find(tabla_paginas, (void *)coincide_pagina);
    return marco;
}

void atender_memoria_cpu()
{

    bool control_key = 1;
    t_buffer *buffer;

    while (control_key)
    {
        int cod_op = recibir_operacion(fd_cpu);
        recibir_int(fd_cpu);
        t_paquete *paquete;
        switch (cod_op)
        {
        case MENSAJE:
            // recibir_mensaje(cliente_fd);
            break;
        case SOLICITUD_INSTRUCCION:
            // buffer = recibir_todo_buffer(fd_cpu);
            atender_peticion_instruccion();
            break;
        case OBTENER_MARCO:
            pidBuscado = recibir_int(fd_cpu);
            int paginaSolicitada = recibir_int(fd_cpu);
            int marcoObtenido = consultar_tabla_paginas(pidBuscado, paginaSolicitada);
            paquete = crear_paquete(OBTENER_MARCO);
            agregar_int_a_paquete(paquete, marcoObtenido);
            enviar_paquete(paquete, fd_cpu);
            break;
        case OBTENER_DATO_MARCO:
            pidBuscado = recibir_int(fd_cpu);
            int marcoSolicitado = recibir_int(fd_cpu);
            int desplazamientoObtenido = recibir_int(fd_cpu);
            log_error(memoria_logger, "obteniendo cant_bytes...");
            size_t cant_bytes = (size_t)recibir_int(fd_cpu);
            log_error(memoria_logger, "cant_bytes obtenido");
            void *buffer;
            void *resultado = operar_memoria(pidBuscado, marcoSolicitado, desplazamientoObtenido, buffer, cant_bytes, 'r');
            paquete = crear_paquete(OBTENER_DATO_MARCO); // ESTO LO IGNORO
            agregar_a_paquete_datoEstatico(paquete, resultado, cant_bytes);
            log_error(memoria_logger, "dato obtenido a enviar: %u", *(uint8_t *)resultado);
            enviar_paquete(paquete, fd_cpu);
            break;
        case -1:
            log_error(memoria_logger, "El CPU se desconecto. Terminando servidor");
            control_key = 0;
            // return EXIT_FAILURE;
            break;
        default:
            log_warning(memoria_logger, "Operacion desconocida de CPU");
            break;
        }
    }
}
