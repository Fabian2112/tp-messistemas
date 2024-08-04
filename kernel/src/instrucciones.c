
#include <../include/instrucciones.h>



void crear_memoria_pcb(int id, char *nombre)
{
	/*kernel_memoria_data *info = malloc(sizeof(*info));
	info->pid = id;
	info->nombre = nombre;
	info->pagina = 0;
	//enviar_kernel_memoria(info, fd_memoria, INICIAR_PROCESO_KERNEL);*/
	t_paquete *paquete=crear_paquete(INICIAR_PROCESO_KERNEL);
	agregar_int_a_paquete(paquete, id);
	agregar_string_a_paquete(paquete,nombre);
	enviar_paquete(paquete,fd_memoria);
	eliminar_paquete(paquete);
	int ok=0;
	recv(fd_memoria, &ok, sizeof(int), MSG_WAITALL);
	sem_post(&existenProcesosNuevos);
}

/*
void execute()
{
    lista_punteros_archivos = list_create();
        
    while (1)
    {
        t_pcb *pcbViejo = list_get(listaProcesosReady, 0);
        pidActual = pcbViejo->pid;
        procesoEjecutandoActual = pcbViejo;

        log_info(kernel_logger, "ejecutando el proceso con pid: %d", pcbViejo->pid);

        enviar_cpu_kernel(pcbViejo, fd_cpu_dispatch, EXECUTE_CPU);

        cpu_ejecutando = true;
        
        log_info(kernel_logger, "esperando respuesta de cpu");

        t_pcb *pcbActual = recibir_cpu_kernel(fd_cpu_dispatch);
        pcbActual->estado = RUNNING;
    }
}


int contador_elementos(char **lista)
{
	int contador = 0;
	while (lista[contador] != NULL)
	{
		contador++;
	}
	return contador;
}

void recurso_inexistente(char *recursoPcb, t_pcb *pcb)
{
	pthread_mutex_lock(&mutexLoggerKernel);
	log_error(kernel_logger, "El recurso %s es inexistente", recursoPcb);
	pthread_mutex_unlock(&mutexLoggerKernel);
}

int buscar_posicion_recurso(char *recurso)
{
	t_queue **listaRecursos = RECURSOS; // se guarda la lista en una variable para su uso
	int tamanioLista = contador_elementos(listaRecursos); // tamanio de la lista recursos
	char *nuevaLinea = strchr(recurso, '\n');

	if (nuevaLinea != NULL)
	{
		*nuevaLinea = '\0';
	}
	for (int i = 0; i < tamanioLista; i++)
	{
		if (strcmp(listaRecursos[i], recurso) == 0)
		{ 
			pthread_mutex_lock(&mutexLoggerKernel);
			log_info(kernel_logger, "se encontro la posicion del recurso %s en %d", recurso, i);
			pthread_mutex_unlock(&mutexLoggerKernel);
			return i;
		}
	}
	return -1;
}


char *wait(t_pcb *pcb)
{
	t_queue **listaRecursos = RECURSOS;
	char *recursoDelPcb = pcb->recurso;
	int resultadoWait = buscar_posicion_recurso(recursoDelPcb);
	pthread_mutex_lock(&mutexLoggerKernel);
	log_info(kernel_logger, "encontre posicion recurso %d", resultadoWait);
	pthread_mutex_unlock(&mutexLoggerKernel);
	// chequeo si el elemento existe
	if (resultadoWait == -1)
	{
		log_error(kernel_logger, "ERROR: recurso solicitado no se encuentra en el sistema :(");
		recurso_inexistente(recursoDelPcb, pcb);
		return "no existe recurso";
	}
	else
	{
		log_info(kernel_logger, "Se encontr{o} el recurso solicitado por la instruccion Wait, se comprueba si hay suficientes para la realizar la operacion");
	}

	log_info(kernel_logger, "cantidad de instancias del recurso %d", INSTANCIAS_RECURSOS[resultadoWait]);
	INSTANCIAS_RECURSOS[resultadoWait] -= 1;

	if (INSTANCIAS_RECURSOS[resultadoWait] < 0)
	{
		log_warning(kernel_logger, "Instancias del recurso %s son negativas: %d", recursoDelPcb, INSTANCIAS_RECURSOS[resultadoWait]);
		pcb->programCounter -= 1;
		log_info(kernel_logger, "PUSHEANDO PID %d A COLA N° %d - RECURSO %s", pcb->pid, resultadoWait, recursoDelPcb);
		queue_push(listaRecursos[ resultadoWait], pcb);
		return "Faltan recursos";
	}
	else
	{
		list_add(pcb->recurso_utilizado, recursoDelPcb);
		log_info(kernel_logger, "PID: %d - Wait: %s - Instancias: %d", pcb->pid, pcb->recurso, INSTANCIAS_RECURSOS[resultadoWait]);
		return "Hay recursos suficiente";
	}
}

int elementos_max_signal(int posicion)
{
	if (INSTANCIAS_RECURSOS[posicion] >= RECURSOS_INICIALES[posicion])
	{
		return -1;
	}
	else
	{
		return 1;
	}
}

int en_espera_por_recurso(char *recurso, t_list *list) {
	for (int i = 0; i < list_size(list); i++) {
		t_pcb *aux = (t_pcb*)list_get(list, i);
		if (strcmp(recurso, aux->recurso_solicitado) == 0) {
			return 1;
		}
	}
	return 0;
}

t_pcb* tomar_del_waiting_queue(int recurso_pos, t_pcb *pcb_, t_list *list)
{	
	
	t_queue **listaRecursos = RECURSOS;
	t_pcb *pcb;

	int waiting = en_espera_por_recurso(pcb ->recurso, list);
	if (waiting == 0) return NULL;


	t_list *pcbs = list_create();
	t_queue *queue = listaRecursos[recurso_pos];

	while (queue_size(queue) > 0) {
		pcb = queue_pop(queue);
		if (pcb == NULL) break;
		if (pcb->pid != pcb_->pid && strcmp(pcb_->recurso,pcb->recurso_solicitado) == 0) break;
		list_add(pcbs, pcb);
	}

	for (int i = 0; i < list_size(pcbs); i++) {
		queue_push(queue, list_get(pcbs, i));
	}

	return pcb;
}

char *i_signals(t_pcb *pcb1)
{
	char *recursoDelPcb = pcb1->recurso;
	int resultadoSignal = buscar_posicion_recurso(recursoDelPcb);
    
	if (resultadoSignal == -1)
	{
		log_error(kernel_logger, "ERROR: recurso solicitado no se encuentra en el sistema");
		recurso_inexistente(recursoDelPcb, pcb1);
		return "No existe recurso";
	}
	else
	{
		log_info(kernel_logger, "Se encontró el recurso solicitado por la instruccion Signal");
	}

	int resultadoMax = elementos_max_signal(resultadoSignal);
	if (resultadoMax == -1)
	{
		log_error(kernel_logger, "ERROR: cantidad maxima de recursos alcanzada");
		log_error(kernel_logger, "RECURSOS MAXIMOS PARA PCB %d - Recurso %s - Cantidad %d", pcb1->pid, pcb1->recurso, INSTANCIAS_RECURSOS[resultadoSignal]);
		pcb1->programCounter -=1 ;
		return "Maximo de recursos";
	}

	INSTANCIAS_RECURSOS[resultadoSignal] = +1;
	log_info(kernel_logger, "PID: %d - Signal: %s - Instancias: %d", pcb1->pid, recursoDelPcb, INSTANCIAS_RECURSOS[resultadoSignal]);

	for (int i = 0; i < list_size(pcb1->recurso_utilizado); i++) {
		char *recurso = list_get(pcb1->recurso_utilizado, i);
		if (strcmp(recurso, recursoDelPcb) == 0) {
			list_remove(pcb1->recurso_utilizado, i);
			log_warning(kernel_logger, "REMOVIENDO EL RECURSO %s DEL PCB %d", recurso, pcb1->pid);
		}
	}

	log_info(kernel_logger, "POP EN LA COLA N° %d PARA PID %d. RECURSO %s", resultadoSignal, pcb1->pid, recursoDelPcb);

	int recurso_pos = buscar_posicion_recurso(recursoDelPcb);

	t_pcb *pcb_waiting = (t_pcb*)tomar_del_waiting_queue(recurso_pos, pcb1, blocked);
	
    if (pcb_waiting != NULL && pcb_waiting->pid != pcb1->pid && !finProcesoEjecutando) {
		log_info(kernel_logger, "Se encuentra PCB %d esperando por recurso: %s", pcb_waiting->pid, recursoDelPcb);
		
        //pcb_waiting = pop_blocked();
		//cambio_de_estado();
		//push_estado();

	}

	return "Recurso Liberado";
}

*/