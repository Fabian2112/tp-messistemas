#include <../include/cpu_memoria.h>

// agarrar cantPaginasABuscar (dividir tamBuscado / tamPagina) y buscar cantPaginasABuscar subsiguientes para conseguir cada marco necesario
// y conseguir la informacion que quiero

void atender_cpu_memoria()
{

    bool control_key = 1;
    // t_buffer* un_buffer;

    while (control_key)
    {
        /*int cod_op = recibir_operacion(fd_memoria);
        switch (cod_op) {
            case MENSAJE:
                // recibir_mensaje(cliente_fd);
                break;
            case PAQUETE:
                // lista = recibir_paquete(fd_kernel_INTERRUPT);
                // log_info(cpu_logger, "Me llegaron los siguientes valores:\n");
                // list_iterate(lista, (void*) iterator);
                break;
            // case CREAR_PROCESO_CPU_MEMORIA:
            //     log_info(cpu_logger, "Creado proceso desde el CPU a MEMORIA");
            //     un_buffer = recibir_todo_buffer(fd_memoria);


            //     break;
            case -1:
                log_error(cpu_logger, "La MEMORIA se desconecto. Terminando servidor");
                control_key=0;
                // return EXIT_FAILURE;
                break;
            default:
                log_warning(cpu_logger,"Operacion desconocida de MEMORIA");
                break;
            }*/
    }
}

// la cpu es quien realiza las operaciones, la memoria solo guarda cosas,
// para evitar multiples accesos a memoria (uno para preguntar el marco a traves de un pid y un numero de pagina)
// el otro para buscar realmente que hay guardado alli
// se intenta buscar en TLB

pagina_desplazamiento obtenerPaginaDesplazamiento(void *direccionLogica, int tamPagina)
{
    size_t size = sizeof(uint8_t);
    uintptr_t direccionLogicaUint = (uintptr_t)direccionLogica; // Tamaño por defecto (por ejemplo, uint8_t)
    if (direccionLogicaUint % sizeof(uint32_t) == 0)
    {
        size = sizeof(uint32_t); // Si la dirección es divisible por sizeof(uint32_t), entonces es uint32_t*
    }
    int direccionLogicaInt;
    if (size == sizeof(uint8_t))
    {
        direccionLogicaInt = (int)(*(uint8_t *)direccionLogicaUint);
    }
    else if (size == sizeof(uint32_t))
    {
        direccionLogicaInt = (int)(*(uint32_t *)direccionLogicaUint);
    }
    pagina_desplazamiento paginaDesplazamiento;
    // int direccionLogica2= *(int*)(uintptr_t) direccionLogica;
    paginaDesplazamiento.nroPagina = (int)floor(direccionLogicaInt / tamPagina);
    paginaBuscada = paginaDesplazamiento.nroPagina;
    paginaDesplazamiento.desplazamiento = direccionLogicaInt - paginaDesplazamiento.nroPagina * tamPagina;
    paginaBuscada = paginaDesplazamiento.nroPagina;
    log_error(cpu_logger, "direeccionLogica: %d, pagina: %d, desplazamiento: %d", direccionLogicaInt, paginaBuscada, paginaDesplazamiento.desplazamiento);
    return paginaDesplazamiento;
}

int obtenerNumeroMarco(int pid, int nroPagina)
{
    // buscar en lista TLB por pid y nroPagina
    // if lo encuentra, lo devuelve
    // else: preguntar a memoria y agregar a lista tlb segun algoritmo
    int marco = obtenerMarcoTLB(pid, nroPagina);
    if (marco == -1)
    {
        log_info(cpu_logger, "PID: %d - TLB MISS - Pagina: %d", pid, nroPagina);
        marco = buscarMarcoEnTablaPaginas(pid, nroPagina);
        reemplazarListaTlbSegunAlgoritmo(pid, nroPagina, marco);
    }
    else
    {
        log_info(cpu_logger, "PID: %d - TLB HIT - Pagina: %d", pid, nroPagina);
    }
    log_info(cpu_logger, "PID: %d - OBTENER MARCO - Página: %d - Marco: %d", pid, nroPagina, marco);
    return marco;
}

int obtenerMarcoTLB(int pid, int nroPagina)
{
    int marco = -1;
    TLB *tlbPtr = list_find(listaTLB, (void *)matchesPidAndPage);
    if (tlbPtr != NULL)
    {
        marco = tlbPtr->marco;
    }
    return marco;
}

bool matchesPidAndPage(TLB *tlb)
{

    return tlb->pid == pid && tlb->pagina == paginaBuscada;
}

void reemplazarListaTlbSegunAlgoritmo(int pid, int pagina, int marco)
{
    TLB *tlb = malloc(sizeof(TLB));
    tlb->pid = pid;
    tlb->pagina = pagina;
    tlb->marco = marco;

    tlb->time = temporal_create();
    if (list_size(listaTLB) == CANTIDAD_ENTRADAS_TLB)
    {
        if (strcmp(ALGORITMO_TLB, "FIFO") == 0)
        {
            list_remove(listaTLB, 0);
        }
        else
        {
            TLB *tlbMasViejo = list_get_maximum(listaTLB, (void *)tlb_mayor_tiempo_transcurrido);
            list_remove_element(listaTLB, tlbMasViejo);
        }
    }
    list_add(listaTLB, tlb);
}

void *tlb_mayor_tiempo_transcurrido(TLB *tlb, TLB *tlb2)
{

    return temporal_gettime(tlb->time) >= temporal_gettime(tlb2->time) ? tlb : tlb2;
}

int buscarMarcoEnTablaPaginas(int pid, int pagina)
{
    t_paquete *paquete = crear_paquete(OBTENER_MARCO);
    agregar_int_a_paquete(paquete, pid);
    agregar_int_a_paquete(paquete, pagina);
    enviar_paquete(paquete, fd_memoria);
    recibir_operacion(fd_memoria);
    recibir_int(fd_memoria);
    int marco = recibir_int(fd_memoria);
    return marco;
}

void *buscarDato(void *direccionLogica, size_t tamDatos)
{
    int tamPagina = 32;
    pagina_desplazamiento paginaDesplazamiento = obtenerPaginaDesplazamiento(direccionLogica, tamPagina);
    int marco = obtenerNumeroMarco(pid, paginaDesplazamiento.nroPagina);
    return buscarDatoMemoria(marco, paginaDesplazamiento.desplazamiento, tamDatos);
}

void enviarDato(void *ptrRegistro, void *direccionLogica, size_t tamReg)
{
    int tamPagina = 10;
    pagina_desplazamiento paginaDesplazamiento = obtenerPaginaDesplazamiento(direccionLogica, tamPagina);
    int marco = obtenerNumeroMarco(pid, paginaDesplazamiento.nroPagina);
    t_paquete *paquete = crear_paquete(MOV_OUT);
    agregar_int_a_paquete(paquete, pid);
    agregar_a_paquete_datoDinamico(paquete, ptrRegistro, tamReg);
    agregar_int_a_paquete(paquete, marco);
    agregar_int_a_paquete(paquete, paginaDesplazamiento.desplazamiento);
    enviar_paquete(paquete, fd_memoria);
}

void *buscarDatoMemoria(int marco, int desplazamiento, size_t tamDatos)
{
    t_paquete *paquete = crear_paquete(OBTENER_DATO_MARCO);
    agregar_int_a_paquete(paquete, pid);
    agregar_int_a_paquete(paquete, marco);
    agregar_int_a_paquete(paquete, desplazamiento);
    int tamDatosInt = (int)tamDatos;
    agregar_int_a_paquete(paquete, tamDatosInt);
    enviar_paquete(paquete, fd_memoria);
    return recibir_dato_estatico(fd_memoria, tamDatos);
}

void pedirIncrementoMemoria(int tamanio)
{
    t_paquete *paquete = crear_paquete(RESIZE);
    agregar_int_a_paquete(paquete, pid);
    agregar_int_a_paquete(paquete, tamanio);
    enviar_paquete(paquete, fd_memoria);
    free(paquete);
    if (recibir_int(fd_memoria) == OUT_OF_MEMORY)
    {
        // devolver registros al kernel indicando out_of_memory
        // paquete = crear_paquete(OUT_OF_MEMORY);
        // agregar_registros_a_paquete(paquete, registrosCpu);
        // enviar_paquete(paquete, fd_kernel_interrupt);

        // codigoInterrupcion = OUT_OF_MEMORY
        // existe_interrupcion = true;
    }
}