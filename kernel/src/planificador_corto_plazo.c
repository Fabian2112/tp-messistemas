#include "../include/planificador_corto_plazo.h"


void manejadorPlanificadorCortoPlazo(void* arg){
	while(true){
		sem_wait(&existeProcesoReady);
		sem_wait(&runningLibre);
		sem_wait(&planiCorto);
		pthread_mutex_lock(&mutexListaReady);
		takeProcessNext();
		pthread_mutex_unlock(&mutexListaReady);
		t_paquete* paquete = crear_paquete(EXECUTE_CPU);
		agregar_int_a_paquete(paquete, proceso_ejecutando->pid);
		agregar_registros_a_paquete(paquete, proceso_ejecutando->registros_cpu);
		enviar_paquete(paquete, fd_cpu_dispatch);
		eliminar_paquete(paquete);
		//enviarPCB(proceso_ejecutando, fd_cpu_dispatch);
		sem_post(&existeProcesoRunning);
		sem_post(&planiCorto);
	}
}


void takeProcessNext(){
	proceso_ejecutando = (t_pcb*)list_remove(listaProcesosReady, 0);
	log_info(kernel_logger, "PID: %d - Estado Anterior: %s - Estado Actual: %s", proceso_ejecutando->pid, "READY", "RUNNING");
}

