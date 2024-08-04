#include <../include/desalojador.h>


/*

void actualizarContextoEjecucion(int socket){
		t_list* partes_pcb = list_create();
		partes_pcb = recibir_paquete(fd_cpu_interrupt);
		proceso_ejecutando->programCounter = (int)(uintptr_t)list_get(partes_pcb, 0);
		proceso_ejecutando->estado = list_get(partes_pcb, 1);
		proceso_ejecutando->registros_cpu->AX = (uint8_t)(uintptr_t)list_get(partes_pcb, 2);
		proceso_ejecutando->registros_cpu->BX = (uint8_t)(uintptr_t)list_get(partes_pcb, 3);
		proceso_ejecutando->registros_cpu->CX = (uint8_t)(uintptr_t)list_get(partes_pcb, 4);
		proceso_ejecutando->registros_cpu->DX = (uint8_t)(uintptr_t)list_get(partes_pcb, 5);
		proceso_ejecutando->registros_cpu->PC = (uint32_t)(uintptr_t)list_get(partes_pcb, 6);
		proceso_ejecutando->registros_cpu->EAX = (uint32_t)(uintptr_t)list_get(partes_pcb, 7);
		proceso_ejecutando->registros_cpu->EBX = (uint32_t)(uintptr_t)list_get(partes_pcb, 8);
		proceso_ejecutando->registros_cpu->ECX = (uint32_t)(uintptr_t)list_get(partes_pcb, 9);
		proceso_ejecutando->registros_cpu->EDX = (uint32_t)(uintptr_t)list_get(partes_pcb, 10);
		proceso_ejecutando->registros_cpu->SI = (uint32_t)(uintptr_t)list_get(partes_pcb, 11);
		proceso_ejecutando->registros_cpu->DI = (uint32_t)(uintptr_t)list_get(partes_pcb, 12);
}




void desalojador_round_robin(void *arg){
	
	while(true){
		sem_wait(&existeProcesoRunning);
		usleep(QUANTUM);
		int pid = proceso_ejecutando->pid;
		send(fd_cpu_interrupt, &pid, sizeof(int), 0);
		actualizarContextoEjecucion(fd_cpu_interrupt);
		list_add(listaProcesosReady, proceso_ejecutando);
		log_info(kernel_logger, "PID:%d - Desalojado por fin de Quantum", pid);
		sem_post(&existeProcesoReady);
    	sem_post(&runningLibre);
	}
} 

*/