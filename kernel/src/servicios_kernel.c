#include "../include/servicios_kernel.h"



void log_protegido(char* mensaje){
    pthread_mutex_lock(&mlog);
	log_info(kernel_logger, "%s", mensaje);
	pthread_mutex_unlock(&mlog);
	free(mensaje);
}


void inicializar_estructuras(){
    ejecucion_pausada = true;
    deadlock = false;
       
    pthread_mutex_init(&mutex_pid, NULL);
    log_protegido(string_from_format("SEMAFOROS MUTEX INICIALIZADOS"));
    sem_init(&new_disponible, 0, 0);
    sem_init(&ready_disponible, 0, 0);
    sem_init(&procesos_en_ready, 0, 0);
    sem_init(&procesos_en_exit, 0, 0);
    sem_init(&bloqueado, 0, 0);
    sem_init(&nivel_multiprogramacion, 0, 0);  //ESTO LO HABRA QUE CAMBIAR PARA QUE TOME EL DE LA CONFIG
    log_protegido(string_from_format("SEMAFOROS INICIALIZADOS"));

// new_disponible, ready_disponible, procesos_en_ready,
//       nivel_multiprogramacion, procesos_en_exit, bloqueado;
    cola_new = queue_create(); 
    cola_ready = queue_create();      //Cada cola es una lista de pcbs
    cola_blocked = queue_create();
    cola_exit = queue_create();
    
    log_protegido(string_from_format("COLAS CREADAS"));
}


int asignar_pid(){
    int valor_pid;

    pthread_mutex_lock(&mutex_pid);
    valor_pid = identificador_PID;
    identificador_PID++;
    pthread_mutex_unlock(&mutex_pid);

    return valor_pid;
}


t_pcb* inicializar_pcb(){
    t_pcb* pcb = malloc(sizeof(t_pcb));
    if(pcb == NULL){
        perror("No se pudo alocar la memoria para tu PCB");
        exit(EXIT_FAILURE);
    }
    pcb->pid = 0;
    pcb->programCounter = 0;
    pcb->quantum = 0;
    pcb->estado = NEW;
    pcb->recurso = "recurso_default";
    pcb->recurso_solicitado = "recurso_default";//esto tenía aux lo cuán lo tiene ningún sentido, lo cambio a pcb
    pcb->recurso_utilizado = list_create();
    pcb->registros_cpu.AX = 0;
    pcb->registros_cpu.BX = 0;
    pcb->registros_cpu.CX = 0;
    pcb->registros_cpu.DX = 0;
    pcb->registros_cpu.EAX = 0;
    pcb->registros_cpu.EBX = 0;
    pcb->registros_cpu.ECX = 0;
    pcb->registros_cpu.EDX = 0;
    pcb->registros_cpu.SI = 0;
    pcb->registros_cpu.PC = 0;
    pcb->registros_cpu.DI = 0;
    /*pcb->registros_cpu = malloc(sizeof(registros_cpu));
    if(pcb->registros_cpu == NULL){
        perror("Error asignando memoria a los registros del CPU en el PCB");
        exit(EXIT_FAILURE);
    }*/

    return pcb;
}

void inicializar_registros_cpu(t_pcb* pcb){ //agregue * al pcb ya que las flechas indican que se accede a un puntero
    //está función anteriormente devolvía el pcb, pero solo se inicializan quito el t_pcb* y lo cambio por un void
    pcb->registros_cpu.AX = 0;
    pcb->registros_cpu.BX = 0;
    pcb->registros_cpu.CX = 0;
    pcb->registros_cpu.DX = 0;
    pcb->registros_cpu.EAX = 0;
    pcb->registros_cpu.EBX = 0;
    pcb->registros_cpu.ECX = 0;
    pcb->registros_cpu.EDX = 0;
    pcb->registros_cpu.PC = 0;
    pcb->registros_cpu.SI = 0;

}

t_pcb* crear_pcb(){
    
    t_pcb* pcb = inicializar_pcb();
        pcb->pid = asignar_pid();
        pcb->estado = READY;
        pcb->programCounter = 0;
        pcb->quantum = 0;
        inicializar_registros_cpu(pcb);

    log_trace(kernel_logger, "Se ha creado un PCB! PID: [%d], ESTADO: [%s], PC: [%d], Q: [%d]," 
    "bueno los registros se logean mas tarde jeje", 
    pcb->pid, traducir_estado(pcb->estado), pcb->programCounter, pcb->quantum);
    
    return pcb;
}


